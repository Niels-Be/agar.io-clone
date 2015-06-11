//
// Created by niels on 10.06.15.
//

#ifndef SERVER_LOBBYMANAGER_H
#define SERVER_LOBBYMANAGER_H

#include "GlobalDefs.h"
#include "Gamefield.h"

class LobbyManager {
private:
	ServerPtr mServer;
	unordered_map<uint32_t, GamefieldPtr> mLobbys;
	volatile uint32_t mLobbyId = 1;

public:
	LobbyManager(const ServerPtr& mServer);

	void createLobby(const String& name, const Options& options = Options());
private:
	void onConnected(ClientPtr client);
	void onGetLobbys(ClientPtr client, PacketPtr packet);
	void onJoin(ClientPtr client, PacketPtr packet);

};


#endif //SERVER_LOBBYMANAGER_H
