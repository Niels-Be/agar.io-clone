//
// Created by niels on 10.06.15.
//

#include "LobbyManager.h"
#include "Gamefield.h"
#include "Network/Server.h"
#include "Network/Client.h"
#include "Network/AgarPackets.h"

using std::placeholders::_1;
using std::placeholders::_2;

struct Lobby {
	uint32_t id;
	String name;
	uint32_t playerCount;
	Options options;
};
DECLARE_JSON_STRUCT(Lobby, id, name, playerCount, options)

LobbyManager::LobbyManager(const ServerPtr& mServer) : mServer(mServer) {
	mServer->setOnConnected(std::bind(&LobbyManager::onConnected, this, _1));
}


void LobbyManager::createLobby(const String& name, const Options& options) {
	mLobbys[mLobbyId++] = make_shared<Gamefield>(mServer, name, options);
}

void LobbyManager::onConnected(ClientPtr client) {
	//Set Callbacks
	client->on(PID_GetLobbies, std::bind(&LobbyManager::onGetLobbys, this, _1, _2));
	client->on(PID_Join, std::bind(&LobbyManager::onJoin, this, _1, _2));
}

void LobbyManager::onGetLobbys(ClientPtr client, PacketPtr packet) {
	vector<Lobby> lobbys;
	lobbys.reserve(mLobbys.size());
	for(auto& it : mLobbys)
		lobbys.emplace_back(Lobby{it.first, it.second->getName(), it.second->getPlayerCount(), it.second->getOptions()});
	client->emit(make_shared<LobbyPacket>(lobbys));
}

void LobbyManager::onJoin(ClientPtr client, PacketPtr packet) {
	uint32_t id = **(std::dynamic_pointer_cast<JoinPacket>(packet));
	printf("Client joind %d\n", id);
	mLobbys[id]->onJoin(client, packet);
}

