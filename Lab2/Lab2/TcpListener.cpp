#include "TcpListener.h"
#include <sstream>
#include <iostream>
using namespace std;


CTcpListener::CTcpListener(std::string ipAddress, int port, MessageRecievedHandler handler) //HANDLE THESE ARGUMENTS LATER
	: m_ipAddress(ipAddress), m_port(port), MessageReceived(handler)
{

	m_ipAddress = ipAddress;
	m_port = port;

	//do something like m_ipAddress = ipAddress etc?


	// Initialze winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		cerr << "Can't Initialize winsock! Quitting" << endl;
		return;
	}

	// Create a socket
	m_listening = socket(AF_INET, SOCK_STREAM, 0);
	if (m_listening == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting" << endl;
		return;
	}

	//// Bind the ip address and port to a socket
	//sockaddr_in hint;
	//hint.sin_family = AF_INET;
	//hint.sin_port = htons(m_port);
	//hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton .... 
//	
//	char data[50];
//	std::copy(m_ipAddress.begin(), m_ipAddress.end(), data);
////	const char ip[50] = data;
//	m_ip
//
	SOCKADDR_IN hint; //Address that we will bind our listening socket to
	int hintlen = sizeof(hint); //length of the address (required for accept call)
	hint.sin_addr.s_addr = inet_addr(m_ipAddress.c_str()); //Broadcast locally
	hint.sin_port = htons(m_port); //Port
	hint.sin_family = AF_INET; //IPv4 Socket


	bind(m_listening, (sockaddr*)&hint, sizeof(hint));

	// Tell Winsock the socket is for listening 
	listen(m_listening, SOMAXCONN);

	// Create the master file descriptor set and zero it
	
	FD_ZERO(&master);

	// Add our first socket that we're interested in interacting with; the listening socket!
	// It's important that this socket is added for our server or else we won't 'hear' incoming
	// connections 
	FD_SET(m_listening, &master);

	// this will be changed by the \quit command (see below, bonus not in video!)
	running = true;

}

CTcpListener::~CTcpListener()
{
	Cleanup();
}

// Send a message to the specified client
void CTcpListener::Send(int clientSocket, std::string msg)
{
	send(clientSocket, msg.c_str(), msg.size() + 1, 0);
}

// Initialize winsock
bool CTcpListener::Init()
{
	WSAData data;
	WORD ver = MAKEWORD(2, 2);

	int wsInit = WSAStartup(ver, &data);
	// TODO: Inform caller the error that occured

	return wsInit == 0;
}

// The main processing loop
void CTcpListener::Run()
{ 
	while (running)
	{


		fd_set copy = master;

		// See who's talking to us
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		// Loop through all the current connections / potential connect
		for (int i = 0; i < socketCount; i++)
		{
			// Makes things easy for us doing this assignment
			SOCKET sock = copy.fd_array[i];

			// Is it an inbound communication?
			if (sock == m_listening)
			{
				// Accept a new connection
				SOCKET client = accept(m_listening, nullptr, nullptr);

				// Add the new connection to the list of connected clients
				FD_SET(client, &master);

				// Send a welcome message to the connected client
				string welcomeMsg = "Welcome to the Awesome Chat Server!\r\n";
				send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
			}
			else // It's an inbound message
			{
				char buf[4096];
				ZeroMemory(buf, 4096);

				// Receive message
				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0)
				{
					// Drop the client
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else
				{
					// Check to see if it's a command. \quit kills the server
					if (buf[0] == '\\')
					{
						// Is the command quit? 
						string cmd = string(buf, bytesIn);
						if (cmd == "\\quit")
						{
							running = false;
							break;
						}

						// Unknown command
						continue;
					}

					// Send message to other clients, and definiately NOT the listening socket

					for (int i = 0; i < master.fd_count; i++)
					{
						SOCKET outSock = master.fd_array[i];
						if (outSock != m_listening && outSock != sock)
						{
							ostringstream ss;
							ss << "SOCKET #" << sock << ": " << buf << "\r\n";
							string strOut = ss.str();

							send(outSock, strOut.c_str(), strOut.size() + 1, 0);
						}
					}
				}

			}
		}

	}

}

void CTcpListener::Cleanup()
{
	WSACleanup();
}

// Create a socket
SOCKET CTcpListener::CreateSocket()
{
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening != INVALID_SOCKET)
	{
		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(m_port);
		inet_pton(AF_INET, m_ipAddress.c_str(), &hint.sin_addr);

		int bindOk = bind(listening, (sockaddr*)&hint, sizeof(hint));
		if (bindOk != SOCKET_ERROR)
		{
			int listenOk = listen(listening, SOMAXCONN);
			if (listenOk == SOCKET_ERROR)
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}
	}

	return listening;
}

// Wait for a connection
SOCKET CTcpListener::WaitForConnection(SOCKET listening)
{
	SOCKET client = accept(listening, NULL, NULL);
	return client;
}

////
////
////// Initialze winsock
////WSADATA wsData;
////WORD ver = MAKEWORD(2, 2);
////
////int wsOk = WSAStartup(ver, &wsData);
////if (wsOk != 0)
////{
////	cerr << "Can't Initialize winsock! Quitting" << endl;
////	return;
////}
////
////// Create a socket
////SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
////if (listening == INVALID_SOCKET)
////{
////	cerr << "Can't create a socket! Quitting" << endl;
////	return;
////}
////
////// Bind the ip address and port to a socket
////sockaddr_in hint;
////hint.sin_family = AF_INET;
////hint.sin_port = htons(54000);
////hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton .... 
////
////bind(listening, (sockaddr*)&hint, sizeof(hint));
////
////// Tell Winsock the socket is for listening 
////listen(listening, SOMAXCONN);
////
////// Create the master file descriptor set and zero it
////fd_set master;
////FD_ZERO(&master);
////
////// Add our first socket that we're interested in interacting with; the listening socket!
////// It's important that this socket is added for our server or else we won't 'hear' incoming
////// connections 
////FD_SET(listening, &master);
////
////// this will be changed by the \quit command (see below, bonus not in video!)
////bool running = true;
////
////while (running)
////{
////	
////	}
////}
////
////// Remove the listening socket from the master file descriptor set and close it
////// to prevent anyone else trying to connect.
////FD_CLR(listening, &master);
////closesocket(listening);
////
////// Message to let users know what's happening.
////string msg = "Server is shutting down. Goodbye\r\n";
////
////while (master.fd_count > 0)
////{
////	// Get the socket number
////	SOCKET sock = master.fd_array[0];
////
////	// Send the goodbye message
////	send(sock, msg.c_str(), msg.size() + 1, 0);
////
////	// Remove it from the master file list and close the socket
////	FD_CLR(sock, &master);
////	closesocket(sock);
////}
////
////// Cleanup winsock
////WSACleanup();
////
////system("pause");