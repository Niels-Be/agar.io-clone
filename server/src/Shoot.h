//
// Created by niels on 02.06.15.
//

#ifndef AGARIO_SHOOT_H
#define AGARIO_SHOOT_H


#include "MoveableElement.h"

class Shoot : public MoveableElement {
public:
	Shoot(GamefieldPtr mGamefield, uint32_t mId, const Vector& mPosition, const String& mColor,
		  const Vector& direction);

	virtual ElementType getType() const { return ET_Shoot; }

};


#endif //AGARIO_SHOOT_H
