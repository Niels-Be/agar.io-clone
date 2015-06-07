#include <iostream>

#include "Network/Server.h"
#include "Gamefield.h"

using namespace std;

int main() {
	cout << "Hello, World!" << endl;

	ServerPtr s(new Server());

	GamefieldPtr gf(new Gamefield(s));
	//GamefieldPtr gf = make_shared<Gamefield>(s);

	//Set Callbacks

	s->start("0.0.0.0", 3000);
	s->run();
	s->stop();

	return 0;
}