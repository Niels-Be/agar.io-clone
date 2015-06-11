//
// Created by niels on 10.06.15.
//

#include "ItemEffect.h"
#include "Ball.h"



ItemEffect::Creator& ItemEffect::Creator::get() {
	static Creator c;
	return c;
}

void ItemEffect::registerEffect(uint8_t type, ItemEffect::Factory* factory) {
	assert(Creator::get().Effects.find(type) == Creator::get().Effects.end());
	Creator::get().Effects[type].reset(factory);
}

ItemEffectPtr ItemEffect::create(uint8_t type, BallPtr const& ball) {
	assert(Creator::get().Effects.find(type) != Creator::get().Effects.end());
	return std::move(ItemEffectPtr(Creator::get().Effects[type]->create(ball)));
}


class HighGravityEffect : public ItemEffect {
public:
	HighGravityEffect(const BallPtr& ball) : ItemEffect(ball) { }

	ItemType getType() const { return IT_HighGravity; }
};
RegisterItemEffect(IT_HighGravity, HighGravityEffect)


class SniperShootEffect : public ItemEffect {
private:
	uint8_t mCount = 3;
public:
	SniperShootEffect(const BallPtr& ball) : ItemEffect(ball) { }

	virtual bool update(double timediff) {
		return true;
	}

	virtual bool consume() {
		mCount--;
		return mCount > 0;
	}

	ItemType getType() const { return IT_SniperShoot; }
};
RegisterItemEffect(IT_SniperShoot, SniperShootEffect)


class BoosterEffect : public ItemEffect {
private:
	double mTimediff = 0;
public:
	BoosterEffect(const BallPtr& ball) : ItemEffect(ball) {
		mBall->setBoostFactor(3);
	}

	bool update(double diff) {
		mTimediff+=diff;
		if(mTimediff > 5) {
			mBall->setBoostFactor(1);
			return false;
		}
		return true;
	}

	ItemType getType() const { return IT_Booster; }
};
RegisterItemEffect(IT_Booster, BoosterEffect)


class InvincibleEffect : public ItemEffect {
private:
	double mTimediff = 0;
public:
	InvincibleEffect(const BallPtr& ball) : ItemEffect(ball) { }

	bool update(double diff) {
		mTimediff+=diff;
		return mTimediff < 5;
	}

	ItemType getType() const { return IT_Invincible; }
};
RegisterItemEffect(IT_Invincible, InvincibleEffect)


class NoHungerEffect : public ItemEffect {
private:
	double mTimediff = 0;
public:
	NoHungerEffect(const BallPtr& ball) : ItemEffect(ball) { }

	bool update(double diff) {
		mTimediff+=diff;
		return mTimediff < 30;
	}

	ItemType getType() const { return IT_NoHunger; }
};
RegisterItemEffect(IT_NoHunger, NoHungerEffect)


class LowerCooldownEffect : public ItemEffect {
private:
	double mTimediff = 0;
public:
	LowerCooldownEffect(const BallPtr& ball) : ItemEffect(ball) { }

	bool update(double diff) {
		mTimediff+=diff;
		return mTimediff < 30;
	}

	ItemType getType() const { return IT_LowerCooldown; }
};
RegisterItemEffect(IT_LowerCooldown, LowerCooldownEffect)


class FakeEffect : public ItemEffect {
public:
	FakeEffect(const BallPtr& ball) : ItemEffect(ball) {
		int32_t mass = ball->getMass();
		int splitcount = 5;
		if (mass < 100)
			splitcount = mass / 20;
		int32_t newmass = mass / splitcount;
		for (double angle = 0; angle < 2 * M_PI; angle += (2 * M_PI) / splitcount) {
			BallPtr b = ball->splitUp(Vector::FromAngle(angle));
			b->setMass(newmass);
		}
	}

	ItemType getType() const { return IT_Fake; }
};
RegisterItemEffect(IT_Fake, FakeEffect)

