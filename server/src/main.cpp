#include <iostream>

#include "Network/Server.h"
#include "LobbyManager.h"

using namespace std;


int main() {
	cout << "Hello, World!" << endl;

	ServerPtr s(new Server());

	LobbyManager l(s);
	l.createLobby("default");
	Options opt;
	opt.width = 2000;
	opt.height = 2000;
	l.createLobby("small", opt);

	Options opt2;
	opt2.player.startMass = 750;
	l.createLobby("test", opt2);

	s->start("0.0.0.0", 3000);
	s->run();
	s->stop();

	return 0;
}