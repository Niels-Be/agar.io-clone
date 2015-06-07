//
// Created by niels on 02.06.15.
//

#include "Element.h"

ElementData Element::get() const {
	return ElementData {mId, getType(), mColor, "", mPosition.x, mPosition.y, mSize};
}

ElementUpdateData Element::getUpdate() const {
	return ElementUpdateData {mId, mPosition.x, mPosition.y, mSize, 0, 0 };
}
