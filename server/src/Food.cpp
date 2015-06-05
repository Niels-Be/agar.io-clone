//
// Created by niels on 02.06.15.
//

#include "Food.h"
#include "Gamefield.h"

Food::Food(GamefieldPtr mGamefield, uint32_t mId, const Vector& mPosition) :
		Element(mGamefield, mId, mPosition, mGamefield->getOptions().food.color, mGamefield->getOptions().food.size,
				mGamefield->getOptions().food.mass) {

}
