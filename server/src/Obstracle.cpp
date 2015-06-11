//
// Created by niels on 02.06.15.
//

#include "Obstracle.h"
#include "Gamefield.h"
#include "Ball.h"
#include "Shoot.h"

Obstracle::Obstracle(GamefieldPtr mGamefield, uint32_t mId, const Vector& mPosition) :
		MoveableElement(mGamefield, mId, mPosition, mGamefield->getOptions().obstracle.color,
				mGamefield->getOptions().obstracle.size) {
}

bool Obstracle::tryEat(ElementPtr other) {
	if (other->getType() == ET_Ball && other->getMass() > mGamefield->getOptions().obstracle.needMass) {
		BallPtr ball = std::dynamic_pointer_cast<Ball>(other);
		uint32_t mass = ball->getMass() / 2;
		uint splitcount = 10;
		if (mass < mGamefield->getOptions().player.minSplitMass*splitcount)
			splitcount = mass / mGamefield->getOptions().player.minSplitMass;
		uint32_t newmass = mass / splitcount;
		for (double angle = 0; angle < 2 * M_PI; angle += (2 * M_PI) / splitcount) {
			BallPtr b = ball->splitUp(Vector::FromAngle(angle));
			b->setMass(newmass);
		}
		mGamefield->destroyElement(ball);
		mGamefield->destroyElement(std::dynamic_pointer_cast<Element>(shared_from_this()));
		return true;
	} else if (other->getType() == ET_Shoot) {
		mEatCount++;
		if (mEatCount >= mGamefield->getOptions().obstracle.eatCount) {
			//shoot out new Obstracle
			ShootPtr shoot = std::dynamic_pointer_cast<Shoot>(other);
			ObstraclePtr o = mGamefield->createObstracle(mPosition);
			o->setBoost(shoot->getMoveDirection() * mGamefield->getOptions().shoot.speed, mGamefield->getOptions().shoot.acceleration);
			mEatCount = 0;
		}
		//Grow up to a size of 150%
		setSize(mGamefield->getOptions().obstracle.size +
				mGamefield->getOptions().obstracle.size * 0.5 / (mGamefield->getOptions().obstracle.eatCount - 1) *
				mEatCount);
		mGamefield->destroyElement(other);
		return true;
	}
	return false;
}
