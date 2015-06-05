//
// Created by niels on 04.06.15.
//

#ifndef AGARIO_PACKETMANAGER_H
#define AGARIO_PACKETMANAGER_H

#include "GlobalDefs.h"

class PacketManager {
public:
	template<class T>
	struct Register {
		Register(uint8_t packetid) {
			PacketManager::get().registerPacket(packetid, new FactoryImpl<T>());
		}
	};
private:
	class Factory {
	public:
		virtual ~Factory() {}
		virtual PacketPtr create() = 0;
	};
	template<class T>
	class FactoryImpl : public Factory {
		PacketPtr create() {
			return std::make_shared<T>();
		}
	};

	unordered_map<uint8_t, std::unique_ptr<Factory> > mPackets;
public:
	static PacketManager& get();

	PacketPtr create(uint8_t id);
	void registerPacket(uint8_t id, Factory* factory);

private:
	PacketManager() = default;
};

#define RegisterPacket(id, ...) namespace { PacketManager::Register<__VA_ARGS__> __packet_##id(id); }


#endif //AGARIO_PACKETMANAGER_H
