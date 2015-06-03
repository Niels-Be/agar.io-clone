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

BallPtr Gamefield::createBall(PlayerPtr player) {
	BallPtr b = std::make_shared<Ball>(shared_from_this(), mElementIds++, generatePos(), player);
	mElements.push_back(b);
	mNewElements.push_back(b);
	return b;
}

ShootPtr Gamefield::createShoot(const Vector& pos, const String& color, const Vector& direction) {
	ShootPtr s = std::make_shared<Shoot>(shared_from_this(), mElementIds++, pos, color, direction);
	mElements.push_back(s);
	mNewElements.push_back(s);
	return s;
}

void Gamefield::destroyElement(Element* elem) {
	elem->mDeleted = true;

	//Find element
	auto it = mElements.begin();
	while (it != mElements.end())
		if ((*it)->getId() == elem->getId())
			break;
	if (it != mElements.end()) {
		mDeletedElements.push_back(*it);
		//Swap with last element then pop last (no realocation needed)
		(*it) = std::move(mElements.back());
		mElements.pop_back();

		if (elem->getType() == ET_Food)
			mFoodCounter--;
		if (elem->getType() == ET_Obstracle)
			mObstracleCounter--;
	}
}

Vector Gamefield::generatePos() {
	return Vector((rand() / (double) RAND_MAX) * mOptions.width, (rand() / (double) RAND_MAX) * mOptions.height);
}

void Gamefield::update(double timediff) {
	for (ElementPtr e : mElements)
		e->update(timediff);

	checkCollisions(timediff);

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

	//Send updated data
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
 * // Theoretic O(n * log n)
	unordered_map<uint64_t, CollisionStore> elements;

	//Update Objects in collision map
	for(auto&& it : elements)
		it.second.distance -= (it.second.e1->getSpeed() + it.second.e2->getSpeed()) * timediff;

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
	// O(n * log n)
	//Check collisions
	for (size_t i = 0; i < mElements.size(); i++) {
		ElementPtr e1 = mElements[i];
		//Start at i because we already checked elements before
		for (size_t j = i; j < mElements.size(); j++) {
			ElementPtr e2 = mElements[j];
			if (e1->getId() == e2->getId() || e2->isDeleted() || e1->isDeleted())
				continue;
			if (e1->intersect(e2)) {
				doIntersect(e1, e2);
			}
		}
	}
}


void Gamefield::doIntersect(ElementPtr e1, ElementPtr e2) {
	if (e1->tryEat(e2)) {

	} else if (e2->tryEat(e1)) {

	}
}

ElementPtr Gamefield::createFood() {
	ElementPtr f = std::make_shared<Food>(shared_from_this(), mElementIds++, generatePos());
	mElements.push_back(f);
	mNewElements.push_back(f);
	mFoodCounter++;
	return f;
}

ElementPtr Gamefield::createObstracle() {
	ElementPtr o = std::make_shared<Obstracle>(shared_from_this(), mElementIds++, generatePos());
	mElements.push_back(o);
	mNewElements.push_back(o);
	mObstracleCounter++;
	return o;
}

void Gamefield::onConnection(SocketIO* sender, const v8::FunctionCallbackInfo<v8::Value>& params) {
	SocketIO* socket = new SocketIO(params[0]->ToObject());
	setCallbacks(socket);
}

void Gamefield::onJoin(SocketIO* sender, const v8::FunctionCallbackInfo<v8::Value>& params) {

}

void Gamefield::onLeave(SocketIO* sender, const v8::FunctionCallbackInfo<v8::Value>& params) {

}

void Gamefield::onStart(SocketIO* sender, const v8::FunctionCallbackInfo<v8::Value>& params) {
	//TODO color
	PlayerPtr ply = std::make_shared<Player>(shared_from_this(), sender->getId(), "some color");
	mPlayer[sender->getId()] = ply;
	ply->addBall(createBall(ply));
}

void Gamefield::onDisconnect(SocketIO* sender, const v8::FunctionCallbackInfo<v8::Value>& params) {

}

void Gamefield::onGetStats(SocketIO* sender, const v8::FunctionCallbackInfo<v8::Value>& params) {

}

void Gamefield::onSplitUp(SocketIO* sender, const v8::FunctionCallbackInfo<v8::Value>& params) {

}

void Gamefield::onShoot(SocketIO* sender, const v8::FunctionCallbackInfo<v8::Value>& params) {

}

void Gamefield::onUpdateTarget(SocketIO* sender, const v8::FunctionCallbackInfo<v8::Value>& params) {

}

void Gamefield::setCallbacks(SocketIO* socket) {

}
