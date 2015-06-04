//
// Created by niels on 04.06.15.
//

#ifndef AGARIO_PACKETMANAGER_H
#define AGARIO_PACKETMANAGER_H

#include "GlobalDefs.h"

class PacketManager {
private:
	class Factory {
	public:
		virtual PacketPtr create() = 0;
	};
	template<class T>
	class FactoryImpl : public Factory {
		PacketPtr create() {
			return std::make_shared<T>();
		}
	};
	template<class T>
	struct Register {
		Register(uint8_t packetid) {
			PacketManager::get().registerPacket(packetid, new FactoryImpl<T>());
		}
	};

	unordered_map<uint8_t, std::unique_ptr<Factory> > mPackets;
public:
	static PacketManager& get();

	PacketPtr create(uint8_t id);
	void registerPacket(uint8_t id, std::unique_ptr<Factory> factory);

private:
	PacketManager() = default;
};

#define RegisterPacket(id, packet) { namespace { PacketManager::Register<packet> __packet_##id(id); }}


#endif //AGARIO_PACKETMANAGER_H
