#include <iostream>

#include "Server.h"

using namespace std;


int main() {
	cout << "Hello, World!" << endl;

	Server s;
	s.start("0.0.0.0", 3000);
	s.run();
	s.stop();

	return 0;
}