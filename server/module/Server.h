//
// Created by niels on 04.06.15.
//

#ifndef AGARIO_SERVER_H
#define AGARIO_SERVER_H


#include "GlobalDefs.h"

class Server {
friend class ServerImpl;
private:
	class ServerImpl;
	std::unique_ptr<ServerImpl> impl;

	std::function<void (ClientPtr)> mOnConnectCallback;

public:
	Server();
	~Server();

	void start(const String& ip, uint16_t port);
	void run();
	void stop();

	void setOnConnected(std::function<void (ClientPtr)> callback) { mOnConnectCallback = callback; }

	//Send to a specific client
	void emit(uint64_t id, PacketPtr packet);
	//Send to all clients
	void emit(PacketPtr packet);

};


#endif //AGARIO_SERVER_H
