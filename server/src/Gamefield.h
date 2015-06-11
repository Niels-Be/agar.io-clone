//
// Created by niels on 02.06.15.
//

#ifndef AGARIO_GAMEFIELD_H
#define AGARIO_GAMEFIELD_H

#include <thread>
#include "GlobalDefs.h"
#include "Vector.h"
#include "Json/JSONValue.h"
#include "Obstracle.h"


struct Options {
	double width = 5000;
	double height = 5000;
	struct Food {
		String color = "#F1C40F";
		double spawn = 5; // per Sec
		uint32_t max = 500;
		uint32_t mass = 1;
		double size = 5;
	} food;
	struct Player {
		double defaultSize = 15;
		double startMass = 20;
		vector<String> color = { "#EA6153", "#00FFFF", "#7FFF00", "#6495ED", "#9932CC", "#FF00FF", "#FFE4B5", "#000000" };
		double targetForce = 50; // unused
		double acceleration = 5000;
		double maxSpeed = 300;
		double speedPenalty = 0.005;
		double eatFactor = 1.2;
		uint32_t minSplitMass = 20;
		uint32_t starveOffset = 250;
		double starveMassFactor = 0.001; //Percent of mass per sec
	} player;
	struct Shoot {
		uint32_t mass = 10;
		double size = 15;
		double speed = 750;
		double acceleration = 400;
	} shoot;
	struct Obstracle {
		String color = "#00FF00";
		double spawn = 0.05;
		uint32_t max = 7;
		double size = 100;
		uint32_t needMass = 200;
		int eatCount = 7;
	} obstracle;
	struct Item {
		String color = "#0000FF";
		double size = 25;
		double spawn = 0.1;
		uint32_t max = 5;
	} item;
};
DECLARE_JSON_STRUCT(Options::Food, color, spawn, max, mass, size)
DECLARE_JSON_STRUCT(Options::Player, defaultSize, startMass, color, targetForce, acceleration, maxSpeed, speedPenalty, eatFactor, minSplitMass, starveOffset, starveMassFactor)
DECLARE_JSON_STRUCT(Options::Shoot, mass, size, speed, acceleration)
DECLARE_JSON_STRUCT(Options::Obstracle, color, spawn, max, size, needMass, eatCount)
DECLARE_JSON_STRUCT(Options::Item, color, size, spawn, max)
DECLARE_JSON_STRUCT(Options, width, height, food, player, shoot, obstracle, item)


struct FPSControl {
	list<std::chrono::high_resolution_clock::duration> timerUpdate;
	list<std::chrono::high_resolution_clock::duration> timerCollision;
	list<std::chrono::high_resolution_clock::duration> timerOther;
};

class Gamefield : public std::enable_shared_from_this<Gamefield> {
private:
	ServerPtr mServer;
	String mName;
	Options mOptions;
	vector<ElementPtr> mElements;
	volatile uint32_t mElementIds = 0;
	unordered_map<uint64_t, PlayerPtr> mPlayer;
	list<ClientPtr> mClients;

	vector<ElementPtr> mNewElements;
	vector<ElementPtr> mDeletedElements;

	QuadTreePtr mQuadTree;

	double mFoodSpawnTimer = 0;
	volatile uint32_t mFoodCounter = 0;
	double mObstracleSpawnTimer = 0;
	volatile uint32_t mObstracleCounter = 0;
	double  mItemSpawnTimer = 0;
	volatile uint32_t mItemCounter = 0;

	double mElementUpdateTimer = 0;
	volatile bool mUpdaterRunning = false;
	std::thread mUpdaterThread;

	FPSControl mFPSControl;
	mutex mMutexElements;
	mutex mMutexNewElements;
	mutex mMutexDeletedElements;

public:
	Gamefield(ServerPtr server, const String& name, const Options& options = Options());
	~Gamefield();

	const String& getName() const { return mName; }
	inline const Options& getOptions() const { return mOptions; }
	uint32_t getPlayerCount() const { return mPlayer.size(); }

	BallPtr createBall(PlayerPtr const&  player) { return createBall(player, generatePos()); }
	BallPtr createBall(PlayerPtr const&  player, const Vector& position);

	ShootPtr createShoot(const Vector& pos, const String& color, const Vector& direction);

	ObstraclePtr createObstracle() { return createObstracle(generatePos()); }
	ObstraclePtr createObstracle(const Vector& position);


	void destroyElement(ElementPtr const&  elem);

	void sendToAll(PacketPtr packet);

	void onJoin(ClientPtr client, PacketPtr packet);

private:
	Vector generatePos();
	void _destroyElement(ElementPtr const&  elem);

	void startUpdater();
	void updateLoop();

	void update(double timediff);

	void checkCollisions(double timediff);

	void doIntersect(QuadTreeNodePtr e1, QuadTreeNodePtr e2);

	ElementPtr createFood();
	ElementPtr createItem();

	void addElement(ElementPtr const& elem);

	void onDisconnected(ClientPtr client);

	void onLeave(ClientPtr client, PacketPtr packet);

	void onStart(ClientPtr client, PacketPtr packet);

	void onGetStats(ClientPtr client, PacketPtr packet);

};


#endif //AGARIO_GAMEFIELD_H
