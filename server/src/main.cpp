#include <iostream>

#include "Network/Server.h"
#include "LobbyManager.h"

using namespace std;


int main() {
	cout << "Hello, World!" << endl;

	ServerPtr s(new Server());

	LobbyManager l(s);
	l.createLobby("default");

	s->start("0.0.0.0", 3000);
	s->run();
	s->stop();

	return 0;
}