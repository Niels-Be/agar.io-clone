//
// Created by niels on 02.06.15.
//

#include "Gamefield.h"
#include "Element.h"
#include "Player.h"
#include "Ball.h"
#include "Shoot.h"
#include "Food.h"
#include "Obstracle.h"
#include "Network/Client.h"
#include "Network/Server.h"
#include "Network/AgarPackets.h"
#include "QuadTree.h"
#include "Item.h"

#include <thread>

using std::placeholders::_1;
using std::placeholders::_2;

Gamefield::Gamefield(ServerPtr server, const String& name, const Options&  options) : mServer(server), mName(name), mOptions(options) {
	//mQuadTree = make_shared<QuadTree>(Vector(0,0), Vector(mOptions.width,  mOptions.height), std::bind(&Gamefield::doIntersect, this, _1, _2));
	mQuadTree = new QuadTree(Vector(0,0), Vector(mOptions.width,  mOptions.height), std::bind(&Gamefield::doIntersect, this, _1, _2));
}


Gamefield::~Gamefield() {
	mUpdaterRunning = false;
	if(mUpdaterThread.joinable())
		mUpdaterThread.join();
}

BallPtr Gamefield::createBall(PlayerPtr const&  player, const Vector& position) {
	BallPtr b = make_shared<Ball>(shared_from_this(), mElementIds++, position, player);
	addElement(b);
	return b;
}

ShootPtr Gamefield::createShoot(const Vector& pos, const String& color, const Vector& direction) {
	ShootPtr s = make_shared<Shoot>(shared_from_this(), mElementIds++, pos, color, direction);
	addElement(s);
	return s;
}


ObstraclePtr Gamefield::createObstracle(const Vector& position) {
	ObstraclePtr o = std::make_shared<Obstracle>(shared_from_this(), mElementIds++, position);
	addElement(o);
	mObstracleCounter++;
	return o;
}


void Gamefield::destroyElement(ElementPtr const&  elem) {
	if(elem->isDeleted()) {
		fprintf(stderr, "Dubble destory of element!!!!!! %d %p\n", elem->getId(), elem.get());
		assert(false);
		return;
	}
	//printf("Maring as Deleted %d %p\n", elem->getId(), elem.get());
	elem->markDeleted();
	{
		lock_guard<mutex> _lock(mMutexDeletedElements);
		mDeletedElements.push_back(elem);
	}

	if (elem->getType() == ET_Ball) {
		auto ball = std::dynamic_pointer_cast<Ball>(elem);
		ball->getPlayer()->removeBall(ball->getId());
	}
	if (elem->getType() == ET_Food)
		mFoodCounter--;
	else if (elem->getType() == ET_Obstracle)
		mObstracleCounter--;
	else if (elem->getType() == ET_Item)
		mItemCounter--;
}


void Gamefield::sendToAll(PacketPtr packet) {
	for(ClientPtr c : mClients)
		c->emit(packet);
}

Vector Gamefield::generatePos() {
	return Vector((rand() / (double) RAND_MAX) * mOptions.width, (rand() / (double) RAND_MAX) * mOptions.height);
}


void Gamefield::_destroyElement(ElementPtr const&  elem) {
	//Remove Element from QuadTree
	if(elem->getRegion()) {
		if (!elem->getRegion()->remove(elem.get())) {
			fprintf(stderr, "Elem is not inside its own Region %d %p\n", elem->getId(), elem.get());
			if (!mQuadTree->remove(elem.get()))
				fprintf(stderr, "Remove from QuadTree failed for %d %p\n", elem->getId(), elem.get());
		}
	} else
	if(!mQuadTree->remove(elem.get()))
		fprintf(stderr, "Remove2 from QuadTree failed for %d %p\n", elem->getId(), elem.get());
	elem->setRegion(NULL);

	lock_guard<mutex> _lock(mMutexElements);

	//Find element
	auto it = mElements.begin();
	while (it != mElements.end()) {
		if ((*it)->getId() == elem->getId())
			break;
		it++;
	}
	if (it != mElements.end()) {
		//Swap with last element then pop last (no realocation needed)
		(*it) = mElements.back();
		mElements.pop_back();
	}
}

void Gamefield::startUpdater() {
	printf("Starting Updater\n");
	if(mUpdaterRunning) return;
	if(mUpdaterThread.joinable()) {
		mUpdaterThread.join();
	}
	while(mFoodCounter < mOptions.food.max)
		createFood();

	printf("Creating new thread\n");
	mUpdaterThread = (std::thread(std::bind(&Gamefield::updateLoop, this)));
}

void Gamefield::updateLoop() {
	using namespace std::chrono;
	using timer=std::chrono::high_resolution_clock;

	mUpdaterRunning = true;
	timer::duration fps(microseconds((uint64_t)(1e6 /  30)));

	printf("Updater started %lf\n",  duration_cast<duration<double, std::milli> >(fps).count());

	do {
		try {
			double timerFPS = 0;

			timer::duration timestamp = timer::now().time_since_epoch();
			while (mUpdaterRunning) {
				double diff = duration_cast<microseconds>(timer::now().time_since_epoch() - timestamp).count() * 1e-6;
				timestamp = timer::now().time_since_epoch();


				update(diff);

				timerFPS += diff;
				if (timerFPS > 1) {
					//onGetStats(ClientPtr(), PacketPtr());
					timerFPS = 0;
				}

				//Only sleep if timediff > 1 milli sec
				timer::duration sleeptime = fps - (timer::now().time_since_epoch() - timestamp);
				if (sleeptime > milliseconds(1))
					std::this_thread::sleep_for(sleeptime);
				else if (sleeptime < milliseconds(0))
					printf("I am to slow !!!! %lf\n", duration_cast<duration<double, std::milli> >(sleeptime).count());
			}
		} catch (std::exception& e) {
			fprintf(stderr, "ERROR: %s\n", e.what());
		} catch (...) {
			fprintf(stderr, "ERROR: Unkowen error occured");
		}
	} while(mUpdaterRunning);
	printf("Updater Stoped\n");
}

void Gamefield::update(double timediff) {
	using namespace std::chrono;
	using timer=std::chrono::high_resolution_clock;

	timer::duration timerStart = timer::now().time_since_epoch();
	timer::duration timerCollision, timerUpdate;

	{
		vector<ElementPtr> changed;
		{
			//lock_guard<mutex> _lock(mMutexElements);
			for (ElementPtr& e : mElements) {
				e->update(timediff);
				if (e->hasChanged())
					changed.push_back(e);
			}
		}

		timerUpdate = timer::now().time_since_epoch() - timerStart;

		//checkCollisions(timediff);
		mQuadTree->doCollisionCheck();

		timerCollision = timer::now().time_since_epoch() - timerUpdate - timerStart;

		for (auto p : mPlayer)
			p.second->update(timediff);

		mFoodSpawnTimer += timediff;
		if (mFoodSpawnTimer > 1 / mOptions.food.spawn) {
			if (mFoodCounter < mOptions.food.max)
				createFood();
			mFoodSpawnTimer = 0;
		}
		mObstracleSpawnTimer += timediff;
		if (mObstracleSpawnTimer > 1 / mOptions.obstracle.spawn) {
			if (mObstracleCounter < mOptions.obstracle.max)
				createObstracle();
			mObstracleSpawnTimer = 0;
		}
		mItemSpawnTimer += timediff;
		if (mItemSpawnTimer > 1 / mOptions.item.spawn) {
			if (mItemCounter < mOptions.item.max)
				createItem();
			mItemSpawnTimer = 0;
		}

		vector<ElementPtr> tmpNew;
		{
			lock_guard<mutex> _lock(mMutexNewElements);
			tmpNew = std::move(mNewElements);
			mNewElements.clear();
		}
		vector<ElementPtr> tmpDeleted;
		{
			lock_guard<mutex> _lock(mMutexDeletedElements);
			tmpDeleted = std::move(mDeletedElements);
			mDeletedElements.clear();
		}

		//Send updated data
		mElementUpdateTimer += timediff;
		if (mElementUpdateTimer > 1) {
			sendToAll(make_shared<SetElementsPacket>(mElements));
			mElementUpdateTimer = 0;
		}
		else if (tmpNew.size() + tmpDeleted.size() + changed.size() > 0) {
			sendToAll(make_shared<UpdateElementsPacket>(tmpNew, tmpDeleted, changed));
			//printf("Sending Update %ld\n", mElements.size());
		}

		for (ElementPtr& elem : tmpDeleted)
			_destroyElement(elem);
	}

	timer::duration timerOther = timer::now().time_since_epoch() - timerCollision - timerUpdate - timerStart;

	mFPSControl.timerUpdate.push_back(timerUpdate);
	mFPSControl.timerCollision.push_back(timerCollision);
	mFPSControl.timerOther.push_back(timerOther);
	if(mFPSControl.timerUpdate.size() > 60) {
		mFPSControl.timerUpdate.pop_front();
		mFPSControl.timerCollision.pop_front();
		mFPSControl.timerOther.pop_front();
	}
	//printf("End of Frame\n");
}

struct CollisionStore {
	ElementPtr e1;
	ElementPtr e2;
	//Distance of both elements is reduces in every frame by (e1.speed + e2.speed) * timediff
	double distance;
};

bool operator <(const CollisionStore& a, const CollisionStore& b) {
	return a.distance < b.distance;
}

uint64_t hash(uint32_t a, uint32_t b) {
	return a < b ? a << 16 | b : b << 16 | a;
}


void Gamefield::checkCollisions(double timediff) {
/* TODO Check which algorithm is faster
 * // This algorithm is faster if exact collision detection is hard (not our case)
	unordered_map<uint64_t, CollisionStore> elements;

	//Update Objects in collision map
	for(auto&& it : elements)
		it.second.distance -= (it.second.e1->getSpeed() + it.second.e2->getSpeed())*(it.second.e1->getSpeed() + it.second.e2->getSpeed()) * timediff;

	//Insert new Objects into Collision Map
	for(ElementPtr e1 : mNewElements)
		for(ElementPtr e2 : mElements)
			if(e1->getId() != e2->getId())
				elements.emplace(hash(e1->getId(), e2->getId()), {e1, e2,
					e1->getPosition().distanceSquared(e2->getPosition())
				});

	//Check for collisions
	for(auto&& it : elements) {
		if(it.second.e1->isDeleted() || it.second.e2->isDeleted())
			continue;
		if(	it.second.distance <= it.second.e1->getSize()*it.second.e1->getSize() ||
			it.second.distance <= it.second.e2->getSize()*it.second.e2->getSize())
		{
			it.second.distance = it.second.e1->getPosition().distanceSquared(it.second.e2->getPosition());
			if(it.second.distance <= it.second.e1->getSize()*it.second.e1->getSize() ||
			   it.second.distance <= it.second.e2->getSize()*it.second.e2->getSize())
			{ // Now they actually intersect
				doIntersect(it.second.e1, it.second.e2);
			}
		}
	}

	//Remove deleted Objects from Collision map
	for(ElementPtr e1 : mDeletedElements)
		for(ElementPtr e2 : mElements)
			elements.erase(hash(e1->getId(), e2->getId()));
*/
	//Check collisions
	for (size_t i = 0; i < mElements.size(); i++) {
		Element* e1 = mElements[i].get();
		if(e1->isDeleted()) continue;
		if(e1->getType() == ET_Food) continue;
		//Start at i + 1 because we already checked elements before
		for (size_t j = 0; j < mElements.size(); j++) {
			Element* e2 = mElements[j].get();
			if (e1->getId() == e2->getId() || e2->isDeleted())
				continue;
			if (e1->intersect(e2)) {
				doIntersect(e1, e2);
			}
		}
	}
}


void Gamefield::doIntersect(QuadTreeNodePtr ne1, QuadTreeNodePtr ne2) {
	try {
		ElementPtr e1(std::dynamic_pointer_cast<Element>(ne1->shared_from_this()));
		ElementPtr e2(std::dynamic_pointer_cast<Element>(ne2->shared_from_this()));
		if (e1->tryEat(e2)) {

		} else if (e2->tryEat(e1)) {

		}
	} catch (std::bad_weak_ptr& e) {
		printf("Bad Week Ptr %p or %p\n", ne1, ne2);
		assert(false);
	}
}

ElementPtr Gamefield::createFood() {
	ElementPtr f = std::make_shared<Food>(shared_from_this(), mElementIds++, generatePos());
	addElement(f);
	mFoodCounter++;
	return f;
}

ElementPtr Gamefield::createItem() {
	ElementPtr o = std::make_shared<Item>(shared_from_this(), mElementIds++, generatePos());
	addElement(o);
	mItemCounter++;
	return o;
}

void Gamefield::addElement(ElementPtr const& elem) {
	{
		lock_guard<mutex> _lock(mMutexElements);
		mQuadTree->add(elem.get());
		mElements.push_back(elem);
	}
	{
		lock_guard<mutex> _lock(mMutexNewElements);
		mNewElements.push_back(elem);
	}
}

void Gamefield::onDisconnected(ClientPtr client) {
	auto it = mPlayer.find(client->getId());
	if(it != mPlayer.end()) {
		for(BallPtr ball : it->second->getBalls())
			destroyElement(ball);
		mPlayer.erase(it);
	}
	mClients.remove(client);

	if(mClients.empty())
		mUpdaterRunning = false;
}

void Gamefield::onJoin(ClientPtr client, PacketPtr packet) {
	//Set Callbacks
	client->on(PID_Leave, std::bind(&Gamefield::onLeave, this, _1, _2));
	client->on(PID_Start, std::bind(&Gamefield::onStart, this, _1, _2));
	client->on(PID_GetStats, std::bind(&Gamefield::onGetStats, this, _1, _2));
	client->setOnDisconnect(std::bind(&Gamefield::onDisconnected, this, _1));
	//Send all elements
	client->emit(std::make_shared<SetElementsPacket>(mElements));
	//Add to update queue
	mClients.push_back(client);

	if(mUpdaterRunning == false)
		startUpdater();
}

void Gamefield::onLeave(ClientPtr client, PacketPtr packet) {
	//Remove from update queue
	mClients.remove(client);

	if(mClients.empty())
		mUpdaterRunning = false;
}

void Gamefield::onStart(ClientPtr client, PacketPtr packet) {
	auto p = std::dynamic_pointer_cast<StartPacket >(packet);
	String color = mOptions.player.color[rand()%mOptions.player.color.size()];
	printf("Player %s joind the game\n", p->Name.c_str());
	PlayerPtr ply = std::make_shared<Player>(shared_from_this(), client, color, p->Name);
	mPlayer[client->getId()] = ply;
	ply->addBall(createBall(ply));
	ply->updateClient();
	client->emit(std::make_shared<EmptyPacket<PID_Start> >());
}

void Gamefield::onGetStats(ClientPtr client, PacketPtr packet) {
	double timerUpdate = 0;
	double timerCollision = 0;
	double timerOther = 0;
	for(auto it : mFPSControl.timerUpdate)
		timerUpdate += std::chrono::duration_cast<std::chrono::duration<double, std::milli> >(it).count() / mFPSControl.timerUpdate.size();
	for(auto it : mFPSControl.timerCollision)
		timerCollision += std::chrono::duration_cast<std::chrono::duration<double, std::milli> >(it).count() / mFPSControl.timerCollision.size();
	for(auto it : mFPSControl.timerOther)
		timerOther += std::chrono::duration_cast<std::chrono::duration<double, std::milli> >(it).count() / mFPSControl.timerOther.size();

	printf("Timings: Update: %lf Collision: %lf Other: %lf Elements: %ld QuadTreeNodes: %ld\n", timerUpdate, timerCollision, timerOther, mElements.size(), mQuadTree->getChildCount());
	if(client)
		client->emit(std::make_shared<StatsPacket>(timerUpdate, timerCollision, timerOther, (uint32_t)mElements.size(), (uint32_t)mPlayer.size()));
}

