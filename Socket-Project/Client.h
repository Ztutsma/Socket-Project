#pragma once

#include "socket-helper.h"
#include "Parser.h"
#include <regex>
#include <fstream>


class Client
{
public:
	Client(std::string IPAddress, int serverPort);
	~Client();

	bool RequestRegister(std::vector<std::string> args);

	bool RequestDeregister(std::vector<std::string> args);

	bool RequestDHTSetup(std::vector<std::string> args);

	bool RequestJoinDHT(std::vector<std::string> args);

	bool RequestLeaveDHT(std::vector<std::string> args);

	bool RequestRebuildDHT();

	bool RequestDHTTearddown();

	bool RequestQueryDHT(std::vector<std::string> args);

	void QueryDHT(std::vector<std::string> args);

private:
	Socket serverSocket;
	Socket leftSocket;
	Socket rightSocket;
	Socket querySocket;

	// Keep track of peers
	Peer self;
	Peer rightPeer;
	Peer leftPeer;


	// Thread Variables
	std::vector<std::thread> threads;

	// DHT Variables
	std::vector<Peer> dhtPeers;
	int dhtRingSize;

	// Hashtable Node structure and list
	struct HashNode
	{
		std::string countryCode;
		std::string shortName;
		std::string tableName;
		std::string longName;
		std::string alphaCode;
		std::string currencyUnit;
		std::string region;
		std::string wb2Code;
		std::string lastPopCensus;
		HashNode* next;
	};
	std::vector<HashNode*> hashTable;

	void StartClient();

	void BuildLeftSocket();

	void BuildQuerySocket();

	void BuildRightSocket();

	void UpdateRightSocket();

	void ListenLeftPort();

	void ListenQueryPort();

	void HandleMessage(std::string msg);

	void BuildDHTNetwork(std::vector<std::string> args);

	void BuildDHT();

	void RebuildDHT();

	void SetDHTPeerInfo(std::vector<std::string> args);

	void StoreDHTEntry(std::vector<std::string> args);

	std::vector<std::string> SendMessageWResponse(Socket socket, std::string msg);

	std::vector<std::string> SendMessageWResponse(Socket socket, std::vector<std::string> args);

	void SendMessageNoResponse(Socket socket, std::string msg);

	void SendMessageNoResponse(Socket socket, std::vector<std::string> args);
};

bool ValidateIPAddress(std::string input);