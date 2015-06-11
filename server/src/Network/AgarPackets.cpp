//
// Created by niels on 05.06.15.
//

#include "AgarPackets.h"
#include "Packet.h"
#include "PacketManager.h"
#include "Player.h"
#include "Ball.h"
#include "Json/JSON.h"


RegisterPacket(PID_Join, JoinPacket)
RegisterPacket(PID_Leave, EmptyPacket<PID_Leave>)
RegisterPacket(PID_Start, StartPacket)
RegisterPacket(PID_GetLobbies, EmptyPacket<PID_GetLobbies>)
RegisterPacket(PID_UpdateTarget, StructPacket<PID_UpdateTarget, TargetPacket>)
RegisterPacket(PID_SplitUp, EmptyPacket<PID_SplitUp>)
RegisterPacket(PID_Shoot, EmptyPacket<PID_Shoot>)
RegisterPacket(PID_RIP, EmptyPacket<PID_RIP>)
RegisterPacket(PID_GetStats, EmptyPacket<PID_GetStats>)



template <class T>
void applyValue(vector<uint8_t>& dest, const T& d) {
	dest.insert(dest.end(), (uint8_t*)&d, ((uint8_t*)&d)+sizeof(T));
}

template<>
void applyValue<String>(vector<uint8_t>& dest, const String& d) {
	dest.insert(dest.end(), d.begin(), d.end());
	dest.push_back(0);
}

template<>
void applyValue<JSONValue>(vector<uint8_t>& dest, const JSONValue& d) {
	applyValue(dest, d.Stringify(false));
}

template<>
void applyValue<ElementData>(vector<uint8_t>& dest, const ElementData& ed) {
	applyValue(dest, ed.id);
	applyValue(dest, ed.type);
	applyValue(dest, ed.color);
	applyValue(dest, ed.name);
	applyValue(dest, ed.x);
	applyValue(dest, ed.y);
	applyValue(dest, ed.size);
}

template<>
void applyValue<ElementUpdateData>(vector<uint8_t>& dest, const ElementUpdateData& ed) {
	applyValue(dest, ed.id);
	applyValue(dest, ed.x);
	applyValue(dest, ed.y);
	applyValue(dest, ed.size);
	applyValue(dest, ed.velX);
	applyValue(dest, ed.velY);
}


void StartPacket::parseData(const char* data, uint32_t size) {
	Name = String(data, size);
}

void PlayerUpdatePacket::applyData(vector<uint8_t>& buffer) const {
	//Reserve required bytes
	buffer.reserve(sizeof(uint32_t) + sizeof(uint32_t)*player->getBalls().size());
	list<BallPtr> balls = player->getBalls();
	uint32_t mass = 0;
	for(BallPtr b : balls) {
		mass += b->getMass();
	}
	applyValue(buffer, mass);
	for(BallPtr b : balls) {
		applyValue(buffer, b->getId());
	}

}

void SetElementsPacket::applyData(vector<uint8_t>& buffer) const {
	//Reserve an approximation of required bytes
	buffer.reserve(sizeof(ElementData) * Elements.size() + 1);
	for(ElementPtr e : Elements) {
		applyValue(buffer, e->get());
	}
}

void UpdateElementsPacket::applyData(vector<uint8_t>& buffer) const {
	//Reserve an approximation of required bytes
	buffer.reserve(sizeof(uint16_t) +
						sizeof(ElementData) * NewElements.size() +
						sizeof(uint16_t) +
						sizeof(uint32_t) * DeletedElements.size() +
						sizeof(ElementUpdateData) * UpdatedElements.size());

	applyValue(buffer, (uint16_t)NewElements.size());
	for(ElementPtr e : NewElements) {
		applyValue(buffer, e->get());
	}

	applyValue(buffer, (uint16_t)DeletedElements.size());
	for(ElementPtr e : DeletedElements) {
		applyValue(buffer, e->getId());
	}

	for(ElementPtr e : UpdatedElements) {
		applyValue(buffer, e->getUpdate());
	}

}

