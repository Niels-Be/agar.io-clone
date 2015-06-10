//
// Created by niels on 02.06.15.
//

#ifndef AGARIO_OBSTRACLE_H
#define AGARIO_OBSTRACLE_H


#include "MoveableElement.h"

class Obstracle : public MoveableElement {
private:
	int mEatCount = 0;
public:
	Obstracle(GamefieldPtr mGamefield, uint32_t mId, const Vector& mPosition);

	virtual bool tryEat(ElementPtr other);

	virtual ElementType getType() const { return ET_Obstracle; }

};


#endif //AGARIO_OBSTRACLE_H
