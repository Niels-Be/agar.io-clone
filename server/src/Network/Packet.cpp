//
// Created by niels on 04.06.15.
//

#include <string.h>
#include "Packet.h"

String Packet::getData() const {
	vector<uint8_t> buf;
	buf.push_back(getId());
	applyData(buf);
	return String(buf.begin(), buf.end());
}


