#pragma once

#include "socket-helper.h"
#include "Parser.h"


class Server
{
public:
	Server(int port);
	~Server();
	void StartServer();

private:
	// Socket/Message variables
	Socket serverSocket;
	Message message;

	// Thread variables
	std::vector<std::thread> threads;

	// Peer/DHT variables
	//Peer* leader;
	Peer leader;
	Peer cachedPeer;

	std::vector<Peer> peers;
	DHTStatus dhtStatus = None;
	int dhtRingSize = 0;
	
	void ListenToPort();

	std::string HandleMessage(std::string msg);

	std::string RegisterPeer(std::vector<std::string> args);

	std::string DeregisterPeer(std::vector<std::string> args);

	std::string StartDHTSetup(std::vector<std::string> args);

	std::string StartDHTTeardown(std::vector<std::string> args);

	std::string AddDHTPeer(std::vector<std::string> args);

	std::string DelDHTPeer(std::vector<std::string> args);

	std::string UpdateDHTStatus(std::vector<std::string> args);

	std::string HandleDHTQuery(std::vector<std::string> args);

	int GetPeerIndex(std::string uname);
};

