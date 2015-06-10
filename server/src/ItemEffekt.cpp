//
// Created by niels on 10.06.15.
//

#include "ItemEffekt.h"
#include "Ball.h"



ItemEffekt::Creator& ItemEffekt::Creator::get() {
	static Creator c;
	return c;
}

void ItemEffekt::registerEffekt(uint8_t type, ItemEffekt::Factory* factory) {
	assert(Creator::get().Effekts.find(type) == Creator::get().Effekts.end());
	Creator::get().Effekts[type].reset(factory);
}

unique_ptr<ItemEffekt> ItemEffekt::create(uint8_t type) {
	assert(Creator::get().Effekts.find(type) != Creator::get().Effekts.end());
	return std::move(unique_ptr<ItemEffekt>(Creator::get().Effekts[type]->create()));
}


class HighGravityEffekt : public ItemEffekt {
public:

	void applyEffekt(BallPtr ball) {

	}

	ItemType getType() const { return IT_HighGravity; }
};
RegisterItemEffekt(IT_HighGravity, HighGravityEffekt)


class SniperShootEffekt : public ItemEffekt {
public:

	void applyEffekt(BallPtr ball) {

	}

	ItemType getType() const { return IT_SniperShoot; }
};
RegisterItemEffekt(IT_SniperShoot, SniperShootEffekt)


class BoosterEffekt : public ItemEffekt {
public:

	void applyEffekt(BallPtr ball) {

	}

	ItemType getType() const { return IT_Booster; }
};
RegisterItemEffekt(IT_Booster, BoosterEffekt)


class InvincibleEffekt : public ItemEffekt {
public:

	void applyEffekt(BallPtr ball) {

	}

	ItemType getType() const { return IT_Invincible; }
};
RegisterItemEffekt(IT_Invincible, InvincibleEffekt)


class NoHungerEffekt : public ItemEffekt {
public:

	void applyEffekt(BallPtr ball) {

	}

	ItemType getType() const { return IT_NoHunger; }
};
RegisterItemEffekt(IT_NoHunger, NoHungerEffekt)


class LowerCooldownEffekt : public ItemEffekt {
public:

	void applyEffekt(BallPtr ball) {

	}

	ItemType getType() const { return IT_LowerCooldown; }
};
RegisterItemEffekt(IT_LowerCooldown, LowerCooldownEffekt)


class FakeEffekt : public ItemEffekt {
public:

	void applyEffekt(BallPtr ball) {

	}

	ItemType getType() const { return IT_Fake; }
};
RegisterItemEffekt(IT_Fake, FakeEffekt)


