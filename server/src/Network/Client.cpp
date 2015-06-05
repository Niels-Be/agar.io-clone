//
// Created by niels on 04.06.15.
//

#include "Client.h"
#include "Packet.h"
#include "Server.h"

void Client::emit(PacketPtr packet) {
	mServer->emit(mId, packet);
}

void Client::on(uint8_t packetId, Client::HandlerFunction func) {
	mPacketHandler[packetId] = func;
}

void Client::handlePacket(PacketPtr packet) {
	assert(mPacketHandler.find(packet->getId()) != mPacketHandler.end());
	mPacketHandler[packet->getId()](shared_from_this(), packet);
}

void Client::handleDisconnect() {
	if(mOnDisconnectCallback)
		mOnDisconnectCallback(shared_from_this());
}
