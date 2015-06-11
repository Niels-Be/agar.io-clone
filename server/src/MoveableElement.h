//
// Created by niels on 02.06.15.
//

#ifndef AGARIO_MOVEABLEELEMENT_H
#define AGARIO_MOVEABLEELEMENT_H

#include "Element.h"

class MoveableElement : public Element {
protected:
	double mMaxSpeed = 0;

	Vector mVelocity;
	Vector mDirection;
	bool mIsMoving = false;

	Vector mBoostVelocity;
	double mBoostAcceleration = 0;
	double mBoostFactor = 1;

public:
	MoveableElement(GamefieldPtr mGamefield, uint32_t mId, const Vector& mPosition, const String& mColor, double mSize,
					uint32_t mass = 0, double speed = 0);
	virtual ~MoveableElement() {}

	void setDirection(const Vector& direction, bool isMoving = true);

	Vector getMoveDirection() const { return Vector::FromAngle((mVelocity + mBoostVelocity).angle()); }

	bool isMoving() const { return mIsMoving; }

	void setBoost(const Vector& velocity, double acceleration);
	void setBoostFactor(double boost);

	virtual double getSpeed() const;

	virtual ElementUpdateData getUpdate() const;

	virtual void update(double timediff);
};

typedef std::shared_ptr<MoveableElement> MoveableElementPtr;


#endif //AGARIO_MOVEABLEELEMENT_H
