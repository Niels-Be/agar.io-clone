//
// Created by niels on 02.06.15.
//

#include "MoveableElement.h"
#include "Gamefield.h"


MoveableElement::MoveableElement(GamefieldPtr mGamefield, uint32_t mId, const Vector& mPosition, const String& mColor,
								 double mSize, uint32_t mass, double speed) :
		Element(mGamefield, mId, mPosition, mColor, mSize, mass), mMaxSpeed(speed) {

}

void MoveableElement::update(double timediff) {
	Element::update(timediff);

	if (mBoostVelocity.x != 0 || mBoostVelocity.y != 0) {
		double velX = mBoostVelocity.x - mBoostAcceleration * timediff * sign(mBoostVelocity.x);
		double velY = mBoostVelocity.y - mBoostAcceleration * timediff * sign(mBoostVelocity.y);

		mBoostVelocity.x = sign(velX) == sign(mBoostVelocity.x) ? velX : 0;
		mBoostVelocity.y = sign(velY) == sign(mBoostVelocity.y) ? velY : 0;
		changed();
	}

	//double a = mGamefield->getOptions().player.force / (mMass + 1)
	double a = mGamefield->getOptions().player.acceleration; //constant acceleration independent of mass

	if (mIsMoving) {
		Vector vel = mDirection * a * timediff + mVelocity;

		mVelocity.x = abs(vel.x) > abs(mMaxSpeed * mBoostFactor * mDirection.x) ? mMaxSpeed * mBoostFactor * mDirection.x : vel.x;
		mVelocity.y = abs(vel.y) > abs(mMaxSpeed * mBoostFactor * mDirection.y) ? mMaxSpeed * mBoostFactor * mDirection.y : vel.y;
		//printf("%lf, %lf  %lf\n", mVelocity.x, mVelocity.y, timediff);
		changed();
	} else if (mVelocity.x != 0 || mVelocity.y != 0) {
		double velX = mVelocity.x - a * timediff * sign(mVelocity.x);
		double velY = mVelocity.y - a * timediff * sign(mVelocity.y);

		mVelocity.x = sign(velX) == sign(mVelocity.x) ? velX : 0;
		mVelocity.y = sign(velY) == sign(mVelocity.y) ? velY : 0;
		changed();
	}

	if (hasChanged()) {
		//do not let them move outside the gamefield
		mPosition.x = min(max(mPosition.x + (mVelocity.x + mBoostVelocity.x) * timediff, 0.),
						  mGamefield->getOptions().width);
		mPosition.y = min(max(mPosition.y + (mVelocity.y + mBoostVelocity.y) * timediff, 0.),
						  mGamefield->getOptions().height);
		updateRegion();
	}
}

void MoveableElement::setDirection(const Vector& direction, bool isMoving) {
	mIsMoving = isMoving;
	mDirection = direction;
}

void MoveableElement::setBoost(const Vector& velocity, double acceleration) {
	mBoostVelocity = velocity;
	mBoostAcceleration = acceleration;
}

void MoveableElement::setBoostFactor(double boost) {
	mBoostFactor = boost;
}

double MoveableElement::getSpeed() const {
	return (mVelocity + mBoostVelocity).lengthSquared();
}

ElementUpdateData MoveableElement::getUpdate() const {
	ElementUpdateData eud = Element::getUpdate();
	eud.velX = mVelocity.x + mBoostVelocity.x;
	eud.velY = mVelocity.y + mBoostVelocity.y;
	return eud;
}

