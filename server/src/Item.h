//
// Created by niels on 10.06.15.
//

#ifndef SERVER_ITEM_H
#define SERVER_ITEM_H


#include "Element.h"
#include "ItemEffect.h"

class Item : public Element {
private:
	ItemType mItemType;

public:
	Item(GamefieldPtr mGamefield, uint32_t mId, const Vector& mPosition);
	virtual ~Item() {}

	virtual ElementType getType() const { return ET_Item; }
	virtual bool tryEat(ElementPtr other);


	virtual ElementData get() const;
};


#endif //SERVER_ITEM_H
