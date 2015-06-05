//
// Created by niels on 02.06.15.
//

#include "Element.h"

bool Element::intersect(ElementPtr other) {
	double dist = mPosition.distanceSquared(other->mPosition);
	return dist <= mSize * mSize || dist <= other->mSize * other->mSize;
}

ElementData Element::get() const {
	return ElementData {mId, getType(), mColor, ""};
}

ElementUpdateData Element::getUpdate() const {
	return ElementUpdateData {mId, mPosition.x, mPosition.y, mSize, 0, 0 };
}
