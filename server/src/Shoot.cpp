//
// Created by niels on 02.06.15.
//

#include "Shoot.h"
#include "Gamefield.h"

Shoot::Shoot(GamefieldPtr mGamefield, uint32_t mId, const Vector& mPosition, const String& mColor,
			 const Vector& direction) :
		MoveableElement(mGamefield, mId, mPosition, mColor, mGamefield->getOptions().shoot.size,
						mGamefield->getOptions().shoot.mass) {
	setBoost(direction * mGamefield->getOptions().shoot.speed, mGamefield->getOptions().shoot.acceleration);
}
