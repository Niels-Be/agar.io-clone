//
// Created by niels on 02.06.15.
//

#ifndef AGARIO_GAMEFIELD_H
#define AGARIO_GAMEFIELD_H

#include "GlobalDefs.h"
#include "SocketIO.h"
#include "Vector.h"
#include "Player.h"
#include "Element.h"


/*
@defaultOptions:
    width: 5000
    height: 5000
    food:
        color: '#f1c40f'
        spawn: 5 	#per Sec
        max: 500
        mass: 1
        size: 5
    player:
        color: ['#EA6153', '#00FFFF', '#7FFF00', '#6495ED', '#9932CC', '#FF00FF', '#FFE4B5', '#000000']
        size: 15
        force: 50
        acceleration: 1000
        speed: 300
        speedPenalty: 0.005
        eatFactor: 1.2
        minSpitMass: 20
        maxMass: 1000 #0 to disable
    shoot:
        mass: 10
        speed: 750
        acceleration: 400
    obstracle:
        size: 100
        max: 5
        spawn: 0.2 #per Sec
        color: '#00FF00'
 */
struct Options {
	double width = 5000;
	double height = 5000;
	struct {
		String color = "#F1C40F";
		double spawn = 5; // per Sec
		uint32_t max = 500;
		int32_t mass = 1;
		double size = 5;
	} food;
	struct {
		double defaultSize = 15;
		double startMass = 20;
		double targetForce = 50; // unused
		double acceleration = 0.005;
		double maxSpeed = 300;
		double speedPenalty = 0.005;
		double eatFactor = 1.2;
		int32_t minSplitMass = 20;
		int32_t starveOffset = 250;
		double starveMassFactor = 0.005; //Percent of mass per sec
	} player;
	struct {
		int32_t mass = 10;
		double size = 15;
		double speed = 750;
		double acceleration = 400;
	} shoot;
	struct {
		String color = "#00FF00";
		double spawn = 0.05;
		uint32_t max = 5;
		double size = 100;
		int32_t needMass = 200;
		int eatCount = 7;
	} obstracle;

};

class Gamefield : public std::enable_shared_from_this<Gamefield> {
private:
	SocketIO mRoom;
	Options mOptions;
	vector<ElementPtr> mElements;
	uint64_t mElementIds = 0;
	unordered_map<uint64_t, PlayerPtr> mPlayer;

	vector<ElementPtr> mNewElements;
	vector<ElementPtr> mDeletedElements;


	double mFoodSpawnTimer = 0;
	uint32_t mFoodCounter = 0;
	double mObstracleSpawnTimer = 0;
	uint32_t mObstracleCounter = 0;

public:
	Gamefield(const v8::Local<v8::Object>& socket) : mRoom(socket) { }

	BallPtr createBall(PlayerPtr player);

	ShootPtr createShoot(const Vector& pos, const String& color, const Vector& direction);

	void destroyElement(Element* elem);

	void destroyElement(ElementPtr elem) { destroyElement(elem.get()); }

	inline const Options& getOptions() const { return mOptions; }

private:
	Vector generatePos();

	void update(double timediff);

	void checkCollisions(double timediff);

	void doIntersect(ElementPtr e1, ElementPtr e2);

	ElementPtr createFood();

	ElementPtr createObstracle();

	void onConnection(SocketIO* sender, const v8::FunctionCallbackInfo<v8::Value>& params);

	void onJoin(SocketIO* sender, const v8::FunctionCallbackInfo<v8::Value>& params);

	void onLeave(SocketIO* sender, const v8::FunctionCallbackInfo<v8::Value>& params);

	void onStart(SocketIO* sender, const v8::FunctionCallbackInfo<v8::Value>& params);

	void onDisconnect(SocketIO* sender, const v8::FunctionCallbackInfo<v8::Value>& params);

	void onGetStats(SocketIO* sender, const v8::FunctionCallbackInfo<v8::Value>& params);

	void onSplitUp(SocketIO* sender, const v8::FunctionCallbackInfo<v8::Value>& params);

	void onShoot(SocketIO* sender, const v8::FunctionCallbackInfo<v8::Value>& params);

	void onUpdateTarget(SocketIO* sender, const v8::FunctionCallbackInfo<v8::Value>& params);

	void setCallbacks(SocketIO* socket);

};


#endif //AGARIO_GAMEFIELD_H
