//
// Created by niels on 02.06.15.
//

#include "Ball.h"
#include "Gamefield.h"
#include "Shoot.h"
#include "Player.h"

Ball::Ball(GamefieldPtr mGamefield, uint32_t mId, const Vector& mPosition, PlayerPtr player) :
		Ball(mGamefield, mId, mPosition, player, mGamefield->getOptions().player.startMass) {
}

Ball::Ball(GamefieldPtr mGamefield, uint32_t mId, const Vector& mPosition, PlayerPtr player, int32_t mass) :
		MoveableElement(mGamefield, mId, mPosition, player->getColor(), 1, 1, 1),
		mPlayer(player) {
	setMass(mGamefield->getOptions().player.startMass);
}

BallPtr Ball::splitUp(const Vector& direction) {
	BallPtr b = mGamefield->createBall(mPlayer);
	b->setMass(mMass / 2);
	this->setMass(mMass / 2);

	b->mPosition.x = min(max(mPosition.x + direction.x * mSize * 1.6, 0.), mGamefield->getOptions().width);
	b->mPosition.y = min(max(mPosition.y + direction.y * mSize * 1.6, 0.), mGamefield->getOptions().height);
	//TODO deflect on wall

	b->setDirection(direction);
	b->setBoost(direction * mGamefield->getOptions().shoot.speed, mGamefield->getOptions().shoot.acceleration);

	mPlayer->addBall(b);

	return b;
}

ShootPtr Ball::shoot(const Vector& direction) {
	Vector pos(
			min(max(mPosition.x + direction.x * mSize * 1.6, 0.), mGamefield->getOptions().width),
			min(max(mPosition.y + direction.y * mSize * 1.6, 0.), mGamefield->getOptions().height)
	);
	ShootPtr b = mGamefield->createShoot(pos, mColor, direction);
	this->addMass(-mGamefield->getOptions().shoot.mass);

	//TODO deflect on wall
	return b;
}

void Ball::setMass(uint32_t mass) {
	mSize = mGamefield->getOptions().player.defaultSize + 150.0 * log((mass + 150.0) / 150.0);
	mMaxSpeed = mGamefield->getOptions().player.maxSpeed * exp(-mGamefield->getOptions().player.speedPenalty * mass);
	Element::setMass(mass);
}

bool Ball::tryEat(ElementPtr other) {
	if (other->getMass() > 0 && other->getSize() * mGamefield->getOptions().player.eatFactor < mSize) {
		this->addMass(other->getMass());
		mGamefield->destroyElement(other);
		mPlayer->updateClient();
		return true;
	}
	return false;
}

void Ball::update(double timediff) {
	MoveableElement::update(timediff);

	//TODO move mStarvTimer somewhere else because each ball has the same value
	mStarveTimer += timediff;
	if (mStarveTimer > 1) {
		if (mMass > mGamefield->getOptions().player.starveOffset) {
			mStarveMass += mMass * mGamefield->getOptions().player.starveMassFactor;
			addMass(-floor(mStarveMass));
			mStarveMass -= floor(mStarveMass);
			mPlayer->updateClient();
		}
		mStarveTimer = 0;
	}
}

ElementData Ball::get() const {
	ElementData ed = Element::get();
	ed.name = mPlayer->getName();
	return ed;
}
