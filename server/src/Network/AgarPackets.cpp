//
// Created by niels on 05.06.15.
//

#include "AgarPackets.h"
#include "Packet.h"
#include "PacketManager.h"
#include "Player.h"
#include "Ball.h"


RegisterPacket(PID_Join, EmptyPacket<PID_Join>)
RegisterPacket(PID_Leave, EmptyPacket<PID_Leave>)
RegisterPacket(PID_Start, StructPacket<PID_Start, StartPacket>)
RegisterPacket(PID_UpdateTarget, StructPacket<PID_UpdateTarget, TargetPacket>)
RegisterPacket(PID_SplitUp, EmptyPacket<PID_SplitUp>)
RegisterPacket(PID_Shoot, EmptyPacket<PID_Shoot>)
RegisterPacket(PID_RIP, EmptyPacket<PID_RIP>)
RegisterPacket(PID_GetStats, EmptyPacket<PID_GetStats>)


const void* PlayerUpdatePacket::getDataPtr() const {
	list<BallPtr> balls = player->getBalls();
	uint32_t mass = 0;
	char* buf = new char[getDataLength()];
	char* pos = buf + sizeof(uint32_t);
	for(BallPtr b : balls) {
		uint32_t id = b->getId();
		memcpy(pos, &id, sizeof(uint32_t));
		pos += sizeof(uint32_t);
		mass += b->getMass();
	}
	memcpy(buf, &mass, sizeof(mass));

	assert(pos == buf+getDataLength());
	return buf;
}

uint32_t PlayerUpdatePacket::getDataLength() const {
	return sizeof(uint32_t) + sizeof(uint32_t)*player->getBalls().size();
}




const void* SetElementsPacket::getDataPtr() const {
	char* buf = new char[getDataLength()];
	char* pos = buf;
	for(ElementPtr e : Elements) {
		ElementData&& ed = e->get();
		memcpy(pos, &ed, sizeof(ElementData));
		pos+=sizeof(ElementData);
	}

	assert(pos == buf+getDataLength());
	return buf;
}

uint32_t SetElementsPacket::getDataLength() const {
	return sizeof(ElementData) * Elements.size();
}




const void* UpdateElementsPacket::getDataPtr() const {
	char* buf = new char[getDataLength()];
	char* pos = buf;

	uint16_t len = NewElements.size();
	memcpy(pos, &len, sizeof(uint16_t));
	pos += sizeof(uint16_t);
	for(ElementPtr e : NewElements) {
		ElementData&& ed = e->get();
		memcpy(pos, &ed, sizeof(ElementData));
		pos+=sizeof(ElementData);
	}

	len = DeletedElements.size();
	memcpy(pos, &len, sizeof(uint16_t));
	pos += sizeof(uint16_t);
	for(ElementPtr e : DeletedElements) {
		uint32_t id = e->getId();
		memcpy(pos, &id, sizeof(uint32_t));
		pos+=sizeof(uint32_t);
	}

	for(ElementPtr e : UpdatedElements) {
		ElementUpdateData&& ed = e->getUpdate();
		memcpy(pos, &ed, sizeof(ElementUpdateData));
		pos+=sizeof(ElementUpdateData);
	}

	assert(pos == buf+getDataLength());
	return buf;
}

uint32_t UpdateElementsPacket::getDataLength() const {
	return sizeof(uint16_t) + sizeof(ElementData) * NewElements.size() + sizeof(uint16_t) + sizeof(uint32_t) * DeletedElements.size() + sizeof(ElementUpdateData) * UpdatedElements.size();
}
