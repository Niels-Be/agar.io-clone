//
// Created by niels on 02.06.15.
//

#ifndef AGARIO_FOOD_H
#define AGARIO_FOOD_H


#include "Element.h"

class Food : public Element {

public:
	Food(GamefieldPtr mGamefield, uint32_t mId, const Vector& mPosition);

	virtual ElementType getType() const { return ET_Food; }
};

typedef std::shared_ptr<Food> FoodPtr;


#endif //AGARIO_FOOD_H
