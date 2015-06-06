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

enum ElementEntryID : int8_t {
	EEID_type,
	EEID_color,
	EEID_name,
	EEID_pos,
	EEID_size,
	EEID_vel,
};

struct ElementData {
	uint32_t id;
	ElementType type;
	String color;
	String name;
	double x;
	double y;
	double size;
};

struct ElementUpdateData {
	uint32_t id;
	double x;
	double y;
	double size;
	double velX;
	double velY;
};

class Element : public std::enable_shared_from_this<Element> {
	friend class Gamefield;

protected:
	GamefieldPtr mGamefield;
	uint32_t mId;
	Vector mPosition;
	String mColor;
	double mSize;
	uint32_t mMass;

private:
	bool mHasChanged = false;
	bool mDeleted = false;

public:
	Element(GamefieldPtr mGamefield, uint32_t mId, const Vector& mPosition, const String& mColor, double mSize,
			uint32_t mMass = 0) :
			mGamefield(mGamefield), mId(mId), mPosition(mPosition), mColor(mColor), mSize(mSize), mMass(mMass) { }
	virtual ~Element() {}


	uint32_t getId() const { return mId; }

	virtual bool tryEat(ElementPtr other) { return false; }

	void setSize(double size) {
		mSize = size;
	}

	double getSize() const { return mSize; }

	virtual void setMass(int32_t mass) { mMass = mass; changed(); }

	void addMass(int32_t mass) { setMass(mMass + mass); }

	uint32_t getMass() const { return mMass; }

	const Vector& getPosition() const { return mPosition; }

	virtual double getSpeed() const { return 0; }

	bool intersect(ElementPtr other);

	virtual void update(double /*timediff*/) { mHasChanged = false; }

	virtual ElementType getType() const = 0;

	virtual ElementData get() const;

	virtual ElementUpdateData getUpdate() const;

	void changed() { mHasChanged = true; }

	bool hasChanged() const { return mHasChanged; }

	bool isDeleted() const { return mDeleted; }
};


#endif //AGARIO_ELEMENT_H
