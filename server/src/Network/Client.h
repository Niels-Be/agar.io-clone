//
// Created by niels on 04.06.15.
//

#ifndef AGARIO_CLIENT_H
#define AGARIO_CLIENT_H


#include "GlobalDefs.h"

class Client : public std::enable_shared_from_this<Client> {
public:
	typedef std::function<void (ClientPtr, PacketPtr)>  HandlerFunction;
private:
	uint64_t mId;
	Server* mServer;

	unordered_map<uint8_t, HandlerFunction> mPacketHandler;
	std::function<void (ClientPtr)> mOnDisconnectCallback;

public:
	Client(uint64_t mId, Server* mServer) : mId(mId), mServer(mServer) { }


	uint64_t getId() const { return mId; }

	void emit(PacketPtr packet);
	void on(uint8_t packetId, HandlerFunction func);
	void setOnDisconnect(std::function<void (ClientPtr)> callback) { mOnDisconnectCallback = callback; }

	void handlePacket(PacketPtr packet);
	void handleDisconnect();
};


#endif //AGARIO_CLIENT_H
