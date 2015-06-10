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


class ItemEffekt {
public:
	template<class T>
	struct Register {
		Register(uint8_t type) {
			ItemEffekt::registerEffekt(type, new FactoryImpl<T>());
		}
	};
private:
	class Factory {
	public:
		virtual ~Factory() {}
		virtual ItemEffekt* create() = 0;
	};
	template<class T>
	class FactoryImpl : public Factory {
		ItemEffekt* create() {
			return new T();
		}
	};
	class Creator {
	public:
		unordered_map<uint8_t, unique_ptr<Factory> > Effekts;
		static Creator& get();
	};
public:
	virtual void applyEffekt(BallPtr ball) = 0;
	virtual void update(double timediff) {}

	virtual ItemType getType() const = 0;

	static void registerEffekt(uint8_t type, Factory* factory);
	static unique_ptr<ItemEffekt> create(uint8_t type);
};
#define RegisterItemEffekt(id, ...) namespace { ItemEffekt::Register<__VA_ARGS__> __itemEffekt_##id(id); }


#endif //SERVER_ITEMEFFEKT_H
