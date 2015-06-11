//
// Created by niels on 02.06.15.
//

#ifndef AGARIO_ELEMENT_H
#define AGARIO_ELEMENT_H

#include "GlobalDefs.h"
#include "Vector.h"
#include "QuadTree.h"

enum ElementType : int8_t {
	ET_Ball,
	ET_Food,
	ET_Shoot,
	ET_Obstracle,
	ET_Item
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

class Element : public QuadTreeNode {
	//friend class Gamefield;

protected:
	GamefieldPtr mGamefield;
	uint32_t mId;
	String mColor;
	uint32_t mMass;

private:
	bool mHasChanged = false;

public:
	Element(GamefieldPtr mGamefield, uint32_t mId, const Vector& mPosition, const String& mColor, double mSize,
			uint32_t mMass = 0) :
			QuadTreeNode(mPosition, mSize),
			mGamefield(mGamefield), mId(mId), mColor(mColor), mMass(mMass) { }
	virtual ~Element() { /*printf("Element(%s) %d Destruct at %.0lf, %.0lf\n", mColor.c_str(), mId, mPosition.x, mPosition.y);*/ }


	uint32_t getId() const { return mId; }

	virtual bool tryEat(ElementPtr other) { return false; }

	void setSize(double size) {	mSize = size; }

	virtual void setMass(uint32_t mass) { mMass = mass; changed(); }

	void addMass(int32_t mass) { setMass(mMass + mass); }

	uint32_t getMass() const { return mMass; }

	virtual double getSpeed() const { return 0; }

	virtual void update(double /*timediff*/) { mHasChanged = false; }

	virtual ElementType getType() const = 0;

	virtual ElementData get() const;

	virtual ElementUpdateData getUpdate() const;

	void changed() { mHasChanged = true; }

	bool hasChanged() const { return mHasChanged; }

};


#endif //AGARIO_ELEMENT_H
