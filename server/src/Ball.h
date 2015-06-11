//
// Created by niels on 02.06.15.
//

#ifndef AGARIO_BALL_H
#define AGARIO_BALL_H

#include "MoveableElement.h"
#include "ItemEffect.h"

class Ball : public MoveableElement {

private:
	PlayerPtr mPlayer;
	double mStarveTimer = 0;
	double mStarveMass = 0;

	unordered_map<uint8_t, ItemEffectPtr> mItemEffects;

public:
	Ball(GamefieldPtr mGamefield, uint32_t mId, const Vector& mPosition, PlayerPtr player);
	Ball(GamefieldPtr mGamefield, uint32_t mId, const Vector& mPosition, PlayerPtr player, int32_t mass);
	~Ball();

	const PlayerPtr& getPlayer() const { return mPlayer; }

	virtual void setMass(uint32_t mass);

	virtual bool tryEat(ElementPtr ptr);

	BallPtr splitUp(const Vector& direction);

	ShootPtr shoot(const Vector& direction);

	void applyEffect(ItemEffectPtr effect);
	bool hasEffect(ItemType type) const { return mItemEffects.find(type) != mItemEffects.end(); }

	virtual void update(double timediff);

	virtual ElementData get() const;

	virtual ElementType getType() const { return ET_Ball; }
};


#endif //AGARIO_BALL_H
