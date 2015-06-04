//
// Created by niels on 02.06.15.
//

#include "Player.h"
#include "Ball.h"
#include "Gamefield.h"

Player::Player(GamefieldPtr mGamefield, uint32_t mId, const String& mColor) :
		mId(mId), mGamefield(mGamefield), mColor(mColor) {
}

void Player::setTarget(const Vector& target) {
	for (BallPtr ball : mBalls) {
		Vector t = target - (ball->getPosition() - mPosition);
		if (t.lengthSquared() < ball->getSize() * ball->getSize() / 4)
			ball->setDirection(Vector::ZERO, false); //Stop moving
		else
			ball->setDirection(Vector::FromAngle(t.angle()));
	}
}

void Player::splitUp(const Vector& target) {
	for (BallPtr ball : mBalls) {
		if (ball->getMass() > mGamefield->getOptions().player.minSplitMass) {
			Vector t = target - (ball->getPosition() - mPosition);
			ball->splitUp(Vector::FromAngle(t.angle()));
		}
	}
}

void Player::shoot(const Vector& target) {
	for (BallPtr ball : mBalls) {
		if (ball->getMass() > mGamefield->getOptions().player.minSplitMass) {
			Vector t = target - (ball->getPosition() - mPosition);
			ball->shoot(Vector::FromAngle(t.angle()));
		}
	}
}

void Player::addBall(BallPtr ball) {
	mBalls.push_back(ball);
}

void Player::removeBall(BallPtr ball) {
	for (auto it = mBalls.begin(); it != mBalls.end(); it++) {
		if ((*it)->getId() == ball->getId()) {
			mBalls.erase(it);
			break;
		}
	}
	if (mBalls.empty()) {
		//Send RIP
	}
}

void Player::update(double /*timediff*/) {
	mPosition = Vector::ZERO;
	double size = 0;
	//Center of Player in the middle of its balls weighted by size
	for (BallPtr ball : mBalls) {
		mPosition += ball->getPosition() * ball->getSize();
		size += ball->getSize();
	}
	mPosition /= size;
}
