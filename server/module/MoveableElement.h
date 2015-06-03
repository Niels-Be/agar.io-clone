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


public:
	MoveableElement(GamefieldPtr mGamefield, uint32_t mId, const Vector& mPosition, const String& mColor, double mSize,
					uint32_t mass = 0, double speed = 0);

	void setDirection(const Vector& direction, bool isMoving = true);

	bool isMoving() const { return mIsMoving; }

	void setBoost(const Vector& velocity, double acceleration);

	virtual double getSpeed() const;

	virtual v8::Local<v8::Object> get() const;

	virtual v8::Local<v8::Object> getUpdate() const;

	virtual void update(double timediff);
};

typedef std::shared_ptr<MoveableElement> MoveableElementPtr;


#endif //AGARIO_MOVEABLEELEMENT_H
