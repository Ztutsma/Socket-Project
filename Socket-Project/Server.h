#pragma once

#include <iostream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

#define BUFFERMAX 1000

class Server
{
public:
	Server(int port);
	~Server();
	void StartServer();

private:
	enum PeerState
	{
		Free = 0,
		Leader,
		InDHT
	};

	struct Peer
	{
		std::string uname;
		std::string IPAddr;
		int leftPort;
		int rightPort;
		int queryPort;
		PeerState state;
	};

	struct Socket
	{
		int socket;
		struct sockaddr_in address;
		int port;
	};

	struct Message
	{
		std::string inMsg;
		int msgSize;
		char* outMsg;

		struct sockaddr_in address;
		unsigned int addrLen;
		char buffer[BUFFERMAX];
	};

	enum DHTStatus
	{
		None = 0,
		Running,
		Building,
		Rebuilding,
		Teardown
	};

	// Socket/Message variables
	Socket serverSocket;
	Message message;

	// Thread variables
	std::vector<std::thread> threads;

	// Peer/DHT variables
	Peer leader;
	std::vector<Peer> peers;
	DHTStatus dhtStatus = None;
	
	void ListenToPort();

	std::string HandleMessage(std::string msg);

	std::string RegisterPeer(std::vector<std::string> args);

	std::string DeregisterPeer(std::vector<std::string> args);

	std::string StartDHTSetup(std::vector<std::string> args);

	std::string StartDHTTearddown(std::vector<std::string> args);

	std::string UpdateDHTStatus(std::vector<std::string> args);

	std::string AddDHTPeer(std::vector<std::string> args);

	std::string DelDHTPeer(std::vector<std::string> args);

	std::string HandleDHTQuery(std::vector<std::string> args);
};

