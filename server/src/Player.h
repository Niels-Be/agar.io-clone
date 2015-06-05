//
// Created by niels on 02.06.15.
//

#ifndef AGARIO_PLAYER_H
#define AGARIO_PLAYER_H

#include "GlobalDefs.h"
#include "Vector.h"

class Player : public std::enable_shared_from_this<Player> {
private:
	ClientPtr mClient;
	GamefieldPtr mGamefield;
	list<BallPtr> mBalls;
	String mColor;
	Vector mPosition;
	Vector mTarget;
	String mName;

public:


	Player(GamefieldPtr mGamefield, ClientPtr mClient, const String& mColor, const String& mName);

	String getColor() const { return mColor; }
	String getName() const { return mName; }

	void setTarget(const Vector& target);

	void splitUp(const Vector& target);

	void shoot(const Vector& target);

	void addBall(BallPtr ball);

	void removeBall(uint32_t ball);

	list<BallPtr> getBalls() const { return mBalls; }

	void updateClient();

	void update(double timediff);

private:
	void onSplitUp(ClientPtr client, PacketPtr packet);

	void onShoot(ClientPtr client, PacketPtr packet);

	void onUpdateTarget(ClientPtr client, PacketPtr packet);

};


#endif //AGARIO_PLAYER_H
