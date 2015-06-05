//
// Created by niels on 05.06.15.
//

#ifndef SERVER_AGARPACKETS_H
#define SERVER_AGARPACKETS_H

#include "Packet.h"

enum PacketID : uint8_t {
	//Game Control Packets
	PID_Join 			= 0x10,	//Empty
	PID_Leave			= 0x11,	//Empty
	PID_Start			= 0x12,	//Struct

	//Player Packets
	PID_UpdateTarget  	= 0x20,	//Struct
	PID_SplitUp 		= 0x21,	//Empty
	PID_Shoot			= 0x22,	//Empty
	PID_RIP				= 0x23,	//Empty
	PID_PlayerUpdate	= 0x24, //Dynamic

	//Update Packets
	PID_SetElements 	= 0x30,	//Dynamic
	PID_UpdateElements	= 0x31,	//Dynamic

	//Debug Packets
	PID_GetStats 		= 0xF0	//Struct
};

#pragma pack(1)
struct TargetPacket {
	double x;
	double y;
};
struct StatsPacket {
	double update;
	double collision;
	double other;
};
#pragma pack()

class StartPacket : public EmptyPacket<PID_Start> {
public:
	String Name;

public:
	StartPacket() { }

	void parseData(const char* data, uint32_t size);
};

class PlayerUpdatePacket : public Packet {
public:
	PlayerPtr player;

public:
	PlayerUpdatePacket(PlayerPtr player) : player(player) { }

	uint8_t getId() const { return PID_PlayerUpdate; }

	void parseData(const char* data, uint32_t size) {} //Is out packet only

protected:
	void applyData(vector<uint8_t>& buffer) const;
};

class SetElementsPacket : public Packet {
public:
	const vector<ElementPtr>& Elements;

public:
	SetElementsPacket(const vector<ElementPtr>& Elements) : Elements(Elements) { }

	uint8_t getId() const { return PID_SetElements; }

	void parseData(const char* data, uint32_t size) {} //Is out packet only

protected:
	void applyData(vector<uint8_t>& buffer) const;
};

class UpdateElementsPacket : public Packet {
public:
	const vector<ElementPtr>& NewElements;
	const vector<ElementPtr>& DeletedElements;
	const vector<ElementPtr>& UpdatedElements;

private:
	uint32_t mLength;
public:
	UpdateElementsPacket(const vector<ElementPtr>& NewElements, const vector<ElementPtr>& DeletedElements,
						 const vector<ElementPtr>& UpdatedElements) :
			NewElements(NewElements), DeletedElements(DeletedElements), UpdatedElements(UpdatedElements) { }

	uint8_t getId() const { return PID_UpdateElements; }

	void parseData(const char* data, uint32_t size) {} //Is out packet only

protected:
	void applyData(vector<uint8_t>& buffer) const;
};



#endif //SERVER_AGARPACKETS_H
