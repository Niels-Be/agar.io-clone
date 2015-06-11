//
// Created by niels on 04.06.15.
//

#ifndef AGARIO_PACKET_H
#define AGARIO_PACKET_H

#include "GlobalDefs.h"
#include <string.h>

class Packet {
public:
	virtual ~Packet() {}
	virtual uint8_t getId() const = 0;

	String getData() const;

	virtual void parseData(const char* data, uint32_t size) = 0;

protected:
	virtual void applyData(vector<uint8_t>& buffer) const = 0;
};

template<uint8_t ID>
class EmptyPacket : public Packet {
public:
	virtual ~EmptyPacket() {}

	uint8_t getId() const { return ID; }

	virtual void parseData(const char* data, uint32_t size) {}

protected:
	virtual void applyData(vector<uint8_t>& buffer) const { }
};

template <uint8_t ID, class T>
class StructPacket : public Packet {
private:
	T mData;
public:
	StructPacket() {}

	template<class... Args>
	StructPacket(Args&&... args) : mData{std::forward<Args>(args)...} { }

	uint8_t getId() const { return ID; }

	void parseData(const char* data, uint32_t size) {
		assert(size == sizeof(T));
		memcpy(&mData, data, sizeof(T));
	}

	T& operator*() { return mData; }
	const T& operator*() const { return mData; }
	T* operator->() { return &mData; }
	const T* operator->() const { return &mData; }
	T* get() { return &mData; }
	const T* get() const { return &mData; }

protected:
	void applyData(vector<uint8_t>& buffer) const {
		buffer.reserve(1+sizeof(T));
		buffer.insert(buffer.end(), (uint8_t*)&mData, ((uint8_t*)&mData)+sizeof(T));
	}
};


#endif //AGARIO_PACKET_H
