//
// Created by niels on 04.06.15.
//

#include <string.h>
#include "Packet.h"

String Packet::getData() const {
	String buf(sizeof(uint8_t) + getDataLength(), '\0');
	buf[0] = getId();
	memcpy(&buf[1], getDataPtr(), getDataLength());
	return buf;
}


