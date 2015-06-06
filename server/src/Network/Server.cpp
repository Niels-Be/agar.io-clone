//
// Created by niels on 04.06.15.
//

#include "Server.h"
#include "Client.h"
#include "Packet.h"
#include "PacketManager.h"

#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>

#include <iostream>

typedef websocketpp::server<websocketpp::config::asio> WebSocket;

typedef WebSocket::message_ptr message_ptr;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
using websocketpp::connection_hdl;

class Server::ServerImpl {
private:
	WebSocket server;
	unordered_map<uint64_t, ClientPtr> mClients;
	Server* mBase;

public:
	ServerImpl(Server* base);

	void open(const String& ip, uint16_t port);
	void run();
	void close();

	void emit(uint64_t id, String&& message);
	void emit(String&& message);

private:
	void onOpen(connection_hdl hdl);
	void onMessage(connection_hdl hdl, message_ptr msg);
	void onClose(connection_hdl hdl);

	static connection_hdl toHdl(uint64_t id);
	static uint64_t toClientId(connection_hdl id);
};


Server::ServerImpl::ServerImpl(Server* base) : mBase(base) {
	server.clear_access_channels(websocketpp::log::alevel::all);
	server.set_access_channels(websocketpp::log::alevel::connect);
	server.set_access_channels(websocketpp::log::alevel::disconnect);
	server.set_access_channels(websocketpp::log::alevel::fail);

	// Initialize ASIO
	server.init_asio();

	// Register our handlers
	server.set_open_handler(bind(&ServerImpl::onOpen, this, ::_1));
	server.set_message_handler(bind(&ServerImpl::onMessage, this, ::_1, ::_2));
	server.set_close_handler(bind(&ServerImpl::onClose, this, ::_1));
}

void Server::ServerImpl::open(const String& ip, uint16_t port) {
	//TODO ip
	server.set_reuse_addr(true);
	server.listen(port);
	server.start_accept();
}

void Server::ServerImpl::run() {
	server.run();
}

void Server::ServerImpl::close() {
	server.stop();
}


void Server::ServerImpl::emit(uint64_t id, String&& message) {
	try {
		server.send(toHdl(id), message, websocketpp::frame::opcode::BINARY);
	} catch (websocketpp::exception& e) {
		std::cerr << "WebSocket Error:" << e.what() << std::endl;
	}
}

void Server::ServerImpl::emit(String&& message) {
	for(auto it : mClients)
		emit(it.first, std::forward<String>(message));
}

void Server::ServerImpl::onOpen(connection_hdl hdl) {
	ClientPtr client = std::make_shared<Client>(uint64_t(hdl.lock().get()), mBase);
	mClients[toClientId(hdl)] = client;
	if(mBase->mOnConnectCallback)
		mBase->mOnConnectCallback(client);
}

void Server::ServerImpl::onMessage(connection_hdl hdl, message_ptr msg) {
	const String& data = msg->get_payload();
	//First Byte contains id
	PacketPtr packet = PacketManager::get().create(data[0]);
	//Parse Data without id
	packet->parseData(data.c_str()+1, data.length()-1);
	mClients[toClientId(hdl)]->handlePacket(packet);
}

void Server::ServerImpl::onClose(connection_hdl hdl) {
	mClients[toClientId(hdl)]->handleDisconnect();
	mClients.erase(toClientId(hdl));
}


connection_hdl Server::ServerImpl::toHdl(uint64_t id) {
	//TODO this will end in memory leaks
	WebSocket::connection_type* con = (WebSocket::connection_type*)id;
	return connection_hdl(con->shared_from_this());
}

uint64_t Server::ServerImpl::toClientId(connection_hdl id) {
	return (uint64_t)id.lock().get();
}

Server::Server() :
	impl(new ServerImpl(this))
{
}

Server::~Server() {
}

void Server::start(const String& ip, uint16_t port) {
	impl->open(ip, port);
}

void Server::run() {
	impl->run();
}

void Server::stop() {
	impl->close();
}

void Server::emit(uint64_t id, PacketPtr packet) {
	impl->emit(id, packet->getData());
}

void Server::emit(PacketPtr packet) {
	impl->emit(packet->getData());
}
