//
// Created by niels on 02.06.15.
//

#ifndef AGARIO_ELEMENT_H
#define AGARIO_ELEMENT_H

#include "GlobalDefs.h"
#include "Vector.h"

enum ElementType : int8_t {
	ET_Ball,
	ET_Food,
	ET_Shoot,
	ET_Obstracle
};


class Element {
	friend class Gamefield;

protected:
	GamefieldPtr mGamefield;
	uint32_t mId;
	Vector mPosition;
	String mColor;
	double mSize;
	uint32_t mMass;

private:
	bool mSizeHasChanged = false;
	bool mHasChanged = false;
	bool mDeleted = false;

public:
	Element(GamefieldPtr mGamefield, uint32_t mId, const Vector& mPosition, const String& mColor, double mSize,
			uint32_t mMass = 0) :
			mGamefield(mGamefield), mId(mId), mPosition(mPosition), mColor(mColor), mSize(mSize), mMass(mMass) { }


	uint32_t getId() const { return mId; }

	virtual bool tryEat(ElementPtr other) { return false; }

	void setSize(double size) {
		mSize = size;
		mSizeHasChanged = true;
	}

	double getSize() const { return mSize; }

	virtual void setMass(int32_t mass) { mMass = mass; }

	void addMass(int32_t mass) { setMass(mMass + mass); }

	int32_t getMass() const { return mMass; }

	const Vector& getPosition() const { return mPosition; }

	virtual double getSpeed() const { return 0; }

	bool intersect(ElementPtr other);

	virtual void update(double /*timediff*/) { mHasChanged = mSizeHasChanged = false; }

	virtual ElementType getType() const = 0;

	virtual v8::Local<v8::Object> get() const;

	virtual v8::Local<v8::Object> getUpdate() const;

	void changed() { mHasChanged = true; }

	bool hasChanged() const { return mHasChanged; }

	bool isDeleted() const { return mDeleted; }
};


#endif //AGARIO_ELEMENT_H
