/*
Author: Pindrought
Date: 11/13/2015
This is the solution for the server that you should have at the end of tutorial 3.
*/
//
//#pragma comment(lib,"ws2_32.lib")
//#include <WinSock2.h>
//#include <string>
//#include <iostream>
//
//SOCKET Connections[100];
//int TotalConnections = 0;
//
//void ClientHandlerThread(int index) //index = the index in the SOCKET Connections array
//{
//	int bufferlength; //Holds the length of the message a client sent
//	while (true)
//	{
//		recv(Connections[index], (char*)&bufferlength, sizeof(int), NULL); //get buffer length
//		char * buffer = new char[bufferlength]; //Allocate buffer
//		recv(Connections[index], buffer, bufferlength, NULL); //get buffer message from client
//		for (int i = 0; i < TotalConnections; i++) //For each client connection
//		{
//			if (i == index) //Don't send the chat message to the same user who sent it
//				continue; //Skip user
//			send(Connections[i], (char*)&bufferlength, sizeof(int), NULL);//send the buffer length to client at index i
//			send(Connections[i], buffer, bufferlength, NULL);//send the chat message to client at index i
//		}
//		delete[] buffer; //Deallocate buffer to stop from leaking memory
//	}
//}
//
//int main()
//{
//	//Winsock Startup
//	WSAData wsaData;
//	WORD DllVersion = MAKEWORD(2, 1);
//	if (WSAStartup(DllVersion, &wsaData) != 0) //If WSAStartup returns anything other than 0, then that means an error has occured in the WinSock Startup.
//	{
//		MessageBoxA(NULL, "WinSock startup failed", "Error", MB_OK | MB_ICONERROR);
//		return 0;
//	}
//
//	SOCKADDR_IN addr; //Address that we will bind our listening socket to
//	int addrlen = sizeof(addr); //length of the address (required for accept call)
//	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //Broadcast locally
//	addr.sin_port = htons(1111); //Port
//	addr.sin_family = AF_INET; //IPv4 Socket
//
//	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL); //Create socket to listen for new connections
//	bind(sListen, (SOCKADDR*)&addr, sizeof(addr)); //Bind the address to the socket
//	listen(sListen, SOMAXCONN); //Places sListen socket in a state in which it is listening for an incoming connection. Note:SOMAXCONN = Socket Oustanding Max Connections
//
//	SOCKET newConnection; //Socket to hold the client's connection
//	int ConnectionCounter = 0; //# of client connections
//	for (int i = 0; i < 100; i++)
//	{
//		newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen); //Accept a new connection
//		if (newConnection == 0) //If accepting the client connection failed
//		{
//			std::cout << "Failed to accept the client's connection." << std::endl;
//		}
//		else //If client connection properly accepted
//		{
//			std::cout << "Client Connected!" << std::endl;
//			Connections[i] = newConnection; //Set socket in array to be the newest connection before creating the thread to handle this client's socket.
//			TotalConnections += 1; //Incremenent total # of clients that have connected
//			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerThread, (LPVOID)(i), NULL, NULL); //Create Thread to handle this client. The index in the socket array for this thread is the value (i).
//			std::string buftest = "Welcome! This is the message of the day!.";
//			int size = buftest.size(); //Get size of message in bytes and store it in int size
//			send(Connections[i], (char*)&size, sizeof(int), NULL); //send Size of message
//			send(Connections[i], buftest.c_str(), buftest.size(), NULL); //send Message
//		}
//	}
//
//	system("pause");
//	return 0;
//}
//
//
































#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <sstream>

#pragma comment (lib, "ws2_32.lib")

using namespace std;




#include "TcpListener.h"
#include "Qotd.h"



//void Listener_MessageReceived(CTcpListener* listener, int client, string msg);
//
//// This is kinda bad because it's global.
//CQotd quotes("wisdom.txt");
//
//void main()
//{
//	CTcpListener server("127.0.0.1", 54010, Listener_MessageReceived);
//
//	if (server.Init())
//	{
//		server.Run();
//	}
//}
//
//void Listener_MessageReceived(CTcpListener* listener, int client, string msg)
//{
//	if (msg == "QUOTE")
//	{
//		listener->Send(client, quotes.GetRandomQuote());
//	}
//}
//
//



#include <iostream>
#include <string>

#include "TcpListener.h"
#include "Qotd.h"



using namespace std;

void Listener_MessageReceived(CTcpListener* listener, int client, string msg);



void main()
{
	CTcpListener server("127.0.0.1", 54010, Listener_MessageReceived);

	if (server.Init())
	{
		server.Run();
	}
}

void Listener_MessageReceived(CTcpListener* listener, int client, string msg)
{

	listener->Send(client, "Example Listening");
}




//#include <iostream>
//#include <string>
//
//#include "TcpListener.h"
//#include "Qotd.h"
//
//using namespace std;
//
//void Listener_MessageReceived(CTcpListener* listener, int client, string msg);
//
//// This is kinda bad because it's global.
//CQotd quotes("wisdom.txt");
//
//void main()
//{
//	CTcpListener server("127.0.0.1", 54010, Listener_MessageReceived);
//
//	if (server.Init())
//	{
//		server.Run();
//	}
//}
//
//void Listener_MessageReceived(CTcpListener* listener, int client, string msg)
//{
//	if (msg == "QUOTE")
//	{
//		listener->Send(client, quotes.GetRandomQuote());
//	}
//}
//
//

