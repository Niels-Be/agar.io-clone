//
// Created by niels on 04.06.15.
//

#include "PacketManager.h"

PacketManager& PacketManager::get() {
	static PacketManager mgr;
	return mgr;
}

PacketPtr PacketManager::create(uint8_t id) {
	if(mPackets.find(id) != mPackets.end())
		return mPackets[id]->create();
	else {
		fprintf(stderr, "Unknowen Packet ID %d\n", id);
		return PacketPtr();
	}

}

void PacketManager::registerPacket(uint8_t id, PacketManager::Factory* factory) {
	assert(mPackets.find(id) == mPackets.end());
	mPackets[id].reset(factory);
}
