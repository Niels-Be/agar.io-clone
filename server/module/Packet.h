//
// Created by niels on 04.06.15.
//

#ifndef AGARIO_PACKET_H
#define AGARIO_PACKET_H

#include "GlobalDefs.h"

class Packet {
public:
	virtual uint8_t getId() const = 0;

	String getData() const;

	virtual void parseData(const char* data, uint32_t size) = 0;

protected:
	virtual void* getDataPtr() const = 0;
	virtual uint32_t getDataLength() const = 0;
};

template <uint8_t ID, class T>
class StructPacket : public Packet {
private:
	T mData;
public:
	template<class... Args>
	StructPacket(Args&&... args) : T{std::forward(args...)} { }

	uint8_t getId() const { return ID; }

	void parseData(const char* data, uint32_t size) {
		assert(size == sizeof(T));
		memcpy(&mData, data, sizeof(T));
	}

	T& operator*() { return mData; }
	const T& operator*() const { return mData; }
	T& operator->() { return mData; }
	const T& operator->() const { return mData; }

protected:
	void* getDataPtr() const { return &mData; }
	uint32_t getDataLength() const { return sizeof(T); }
};


#endif //AGARIO_PACKET_H
