//
// Created by niels on 02.06.15.
//

#ifndef AGARIO_GAMEFIELD_H
#define AGARIO_GAMEFIELD_H

#include <thread>
#include "GlobalDefs.h"
#include "Vector.h"
#include "Element.h"


struct Options {
	double width = 5000;
	double height = 5000;
	struct {
		String color = "#F1C40F";
		double spawn = 5; // per Sec
		uint32_t max = 500;
		uint32_t mass = 1;
		double size = 5;
	} food;
	struct {
		double defaultSize = 15;
		double startMass = 20;
		double targetForce = 50; // unused
		double acceleration = 5000;
		double maxSpeed = 300;
		double speedPenalty = 0.005;
		double eatFactor = 1.2;
		uint32_t minSplitMass = 20;
		uint32_t starveOffset = 250;
		double starveMassFactor = 0.001; //Percent of mass per sec
	} player;
	struct {
		uint32_t mass = 10;
		double size = 15;
		double speed = 750;
		double acceleration = 400;
	} shoot;
	struct {
		String color = "#00FF00";
		double spawn = 0.05;
		uint32_t max = 5;
		double size = 100;
		uint32_t needMass = 200;
		int eatCount = 7;
	} obstracle;

};

struct FPSControl {
	list<std::chrono::high_resolution_clock::duration> timerUpdate;
	list<std::chrono::high_resolution_clock::duration> timerCollision;
	list<std::chrono::high_resolution_clock::duration> timerOther;
};

class Gamefield : public std::enable_shared_from_this<Gamefield> {
private:
	ServerPtr mServer;
	Options mOptions;
	vector<ElementPtr> mElements;
	volatile uint32_t mElementIds = 0;
	unordered_map<uint64_t, PlayerPtr> mPlayer;
	list<ClientPtr> mClients;

	vector<ElementPtr> mNewElements;
	vector<ElementPtr> mDeletedElements;


	double mFoodSpawnTimer = 0;
	uint32_t mFoodCounter = 0;
	double mObstracleSpawnTimer = 0;
	uint32_t mObstracleCounter = 0;

	double mElementUpdateTimer = 0;
	volatile bool mUpdaterRunning = false;
	std::thread mUpdaterThread;

	FPSControl mFPSControl;

public:
	Gamefield(ServerPtr server);
	~Gamefield();

	BallPtr createBall(PlayerPtr player);

	ShootPtr createShoot(const Vector& pos, const String& color, const Vector& direction);

	void destroyElement(ElementPtr elem);

	void sendToAll(PacketPtr packet);

	inline const Options& getOptions() const { return mOptions; }

private:
	Vector generatePos();
	void _destroyElement(ElementPtr elem);

	void startUpdater();
	void updateLoop();

	void update(double timediff);

	void checkCollisions(double timediff);

	void doIntersect(ElementPtr e1, ElementPtr e2);

	ElementPtr createFood();

	ElementPtr createObstracle();

	void onConnected(ClientPtr client);
	void onDisconnected(ClientPtr client);

	void onJoin(ClientPtr client, PacketPtr packet);

	void onLeave(ClientPtr client, PacketPtr packet);

	void onStart(ClientPtr client, PacketPtr packet);

	void onGetStats(ClientPtr client, PacketPtr packet);

};


#endif //AGARIO_GAMEFIELD_H
