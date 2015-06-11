//
// Created by niels on 10.06.15.
//

#include "Item.h"
#include "Gamefield.h"
#include "Ball.h"

Item::Item(GamefieldPtr mGamefield, uint32_t mId, const Vector& mPosition) :
	Element(mGamefield, mId, mPosition,  mGamefield->getOptions().item.color, mGamefield->getOptions().item.size)
{
	mItemType = (ItemType) (rand() % IT_COUNT);
}


bool Item::tryEat(ElementPtr other) {
	if(other->getType() == ET_Ball) {
		BallPtr ball(std::dynamic_pointer_cast<Ball>(other));
		ball->applyEffect(ItemEffect::create(mItemType, ball));
		mGamefield->destroyElement(std::dynamic_pointer_cast<Element>(shared_from_this()));
		return true;
	}
	return false;
}

ElementData Item::get() const {
	return Element::get();
}
