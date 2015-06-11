//
// Created by niels on 05.06.15.
//

#ifndef SERVER_AGARPACKETS_H
#define SERVER_AGARPACKETS_H

#include <Json/JSONValue.h>
#include "Packet.h"

enum PacketID : uint8_t {
	//Game Control Packets
	PID_Join 			= 0x10,	//Empty / Struct
	PID_Leave			= 0x11,	//Empty
	PID_Start			= 0x12,	//Struct
	PID_GetLobbies 		= 0x13, //Empty

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
	PID_GetStats 		= 0xF0,	//Struct
	PID_Debug			= 0xF1  //Json
};

#pragma pack(1)
struct TargetPacket {
	double x;
	double y;
};
struct StatsPacketStruct {
	double update;
	double collision;
	double other;
	uint32_t elements;
	uint32_t player;
};
#pragma pack()
DECLARE_JSON_STRUCT(StatsPacketStruct, update, collision, other, elements, player)

typedef StructPacket<PID_GetStats, StatsPacketStruct> StatsPacket;
typedef StructPacket<PID_Join, uint32_t> JoinPacket;


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


template<uint8_t ID>
class JSONPacket : public Packet {
public:
	unique_ptr<JSONValue> Value;

	JSONPacket() {}
	JSONPacket(unique_ptr<JSONValue>&& val) : Value(std::move(val)) {}
	template<class T>
	JSONPacket(const T& data) {
		Value.reset(JSONValue::From(data));
	}

	~JSONPacket() {}

	template<class T>
	void get(T& data) {
		Value->Cast(data);
	}

	uint8_t getId() const { return ID; }

	void parseData(const char* data, uint32_t size) {
		Value.reset(JSON::Parse(data));
	}

protected:
	void applyData(vector<uint8_t>& buffer) const {
		String d = Value->Stringify(false);
		buffer.reserve(1 + d.length());
		buffer.insert(buffer.end(), d.begin(), d.end());
		buffer.push_back(0);
	}

};
typedef JSONPacket<PID_GetLobbies> LobbyPacket;
typedef JSONPacket<PID_Debug> DebugPacket;


#endif //SERVER_AGARPACKETS_H
