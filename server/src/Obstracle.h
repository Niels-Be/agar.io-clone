//
// Created by niels on 02.06.15.
//

#ifndef AGARIO_OBSTRACLE_H
#define AGARIO_OBSTRACLE_H


#include "Element.h"

class Obstracle : public Element {
private:
	int mEatCount = 0;
public:
	Obstracle(GamefieldPtr mGamefield, uint32_t mId, const Vector& mPosition);

	virtual bool tryEat(ElementPtr other);

	virtual ElementType getType() const { return ET_Obstracle; }

};

typedef std::shared_ptr<Obstracle> ObstraclePtr;


#endif //AGARIO_OBSTRACLE_H
