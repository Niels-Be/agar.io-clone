//
// Created by niels on 10.06.15.
//

#ifndef SERVER_ITEMEFFEKT_H
#define SERVER_ITEMEFFEKT_H

#include "GlobalDefs.h"

enum ItemType : uint8_t {
	IT_HighGravity, 	//Other Balls around get attracted
	IT_SniperShoot, 	//Can fire some sniper shoots that fly further and will harm other player
	IT_Booster, 		//Gets a temporary speed boost
	IT_Invincible,		//Can not be eaten for a few sec
	IT_NoHunger, 		//Do not lose mass for a while
	IT_LowerCooldown, 	//Reduces split cooldown for some time
	IT_Fake, 			//Split into pices like Obstracle

	IT_COUNT
};


class ItemEffect {
public:
	template<class T>
	struct Register {
		Register(uint8_t type) {
			ItemEffect::registerEffect(type, new FactoryImpl<T>());
		}
	};
private:
	class Factory {
	public:
		virtual ~Factory() {}
		virtual ItemEffect* create(BallPtr const& ball) = 0;
	};
	template<class T>
	class FactoryImpl : public Factory {
		ItemEffect* create(BallPtr const& ball) {
			return new T(ball);
		}
	};
	class Creator {
	public:
		unordered_map<uint8_t, unique_ptr<Factory> > Effects;
		static Creator& get();
	};

protected:
	BallPtr mBall;
public:
	ItemEffect(BallPtr const& ball) : mBall(ball) { }

	//Returns false if the effect is over
	virtual bool update(double timediff) { return false; }
	//Returns false if the effect is over
	virtual bool consume() { return false; }

	virtual ItemType getType() const = 0;

	static void registerEffect(uint8_t type, Factory* factory);
	static ItemEffectPtr create(uint8_t type, BallPtr const& ball);
};
#define RegisterItemEffect(id, ...) namespace { ItemEffect::Register<__VA_ARGS__> __itemEffect_##id(id); }


#endif //SERVER_ITEMEFFEKT_H
