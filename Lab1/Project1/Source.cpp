#include "Net.h"
#include <iostream>
using namespace std;

void playerA(Net &theNet) {
	theNet.setupUDP(28000, "127.0.0.1", 0);

	std::cout << "Player A's send port: " << theNet.getSenderPort() << std::endl;
	std::cout << "Player A's sender IP: " << theNet.getSenderIP() << std::endl;

	theNet.sendData("127.0.0.1", theNet.portNum, "Baeyeon");
	
}

void playerB(Net &theNet) {

	char message[200] = "";


	theNet.setupUDP(28005, "127.0.0.1", 1);

	std::cout << "Player B's sender port: " << theNet.getSenderPort() << std::endl;
	std::cout << "Player B's sender IP: " << theNet.getSenderIP() << std::endl;

//	std::cout << "Player B's own port: " << theNet.portNum() << std::endl;

	
	//std::cout << "Player B's own IP: " << theNet.getSenderIP() << std::endl;




	while (message != "")
	{
		theNet.receiveData("");
	}
	std::cout << "receivo: " << message << std::endl;
}

int main() {
	Net net;

	net.initialise();

	std::string input;

	std::cin >> input;

	if (input == "a") {
		playerA(net);
	}
	else if (input == "b") {
		playerB(net);
	}

	system("PAUSE");

	return 0;
}