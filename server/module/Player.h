//
// Created by niels on 02.06.15.
//

#ifndef AGARIO_PLAYER_H
#define AGARIO_PLAYER_H

#include "GlobalDefs.h"
#include "Vector.h"

class Player {
private:
	uint32_t mId;
	GamefieldPtr mGamefield;
	list<BallPtr> mBalls;
	String mColor;
	Vector mPosition;

public:


	Player(GamefieldPtr mGamefield, uint32_t mId, const String& mColor);

	String getColor() const { return mColor; }

	void setTarget(const Vector& target);

	void splitUp(const Vector& target);

	void shoot(const Vector& target);

	void addBall(BallPtr ball);

	void removeBall(BallPtr ball);

	void update(double timediff);


};


#endif //AGARIO_PLAYER_H
