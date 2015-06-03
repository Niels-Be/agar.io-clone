//
// Created by niels on 02.06.15.
//

#include "Element.h"

bool Element::intersect(ElementPtr other) {
	double dist = mPosition.distanceSquared(other->mPosition);
	return dist <= mSize * mSize || dist <= other->mSize * other->mSize;
}

v8::Local<v8::Object> Element::get() const {
	return v8::Local<v8::Object>();
}

v8::Local<v8::Object> Element::getUpdate() const {
	if (mSizeHasChanged);
	return v8::Local<v8::Object>();
}
