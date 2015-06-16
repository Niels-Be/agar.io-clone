//
// Created by niels on 02.06.15.
//

#include "Element.h"
#include "Gamefield.h"

Element::Element(GamefieldPtr mGamefield, uint32_t mId, const Vector& mPosition, const String& mColor, double mSize,
				 uint32_t mMass) :
		QuadTreeNode(mGamefield->getQuadTree(), mPosition, mSize),
		mGamefield(mGamefield), mId(mId), mColor(mColor), mMass(mMass)
{
}


ElementData Element::get() const {
	return ElementData {mId, getType(), mColor, "", mPosition.x, mPosition.y, mSize};
}

ElementUpdateData Element::getUpdate() const {
	return ElementUpdateData {mId, mPosition.x, mPosition.y, mSize, 0, 0 };
}
