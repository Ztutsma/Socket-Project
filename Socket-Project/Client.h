// Client.h provides all includes and the class declaration for
// Client objects.
// 
// Name: Collin Amstutz
// Student ID: 1208893539
// Class: CSE 434
//

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

	bool RequestSetupDHT(std::vector<std::string> args);

	bool RequestJoinDHT(std::vector<std::string> args);

	bool RequestLeaveDHT(std::vector<std::string> args);

	bool RequestTearddownDHT(std::vector<std::string> args);

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
	Peer newPeer;

	// Thread Variables
	std::vector<std::thread> threads;
	bool leftDht = false;
	bool joinedDht = false;
	bool killDHT = false;
	bool dhtDead = false;

	// DHT Variables
	std::vector<Peer> dhtPeers;
	int dhtRingSize;
	DHTStatus dhtStatus = None;

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

	// Private Methods
	void StartClient();

	void BuildLeftSocket();

	void BuildQuerySocket();

	void BuildRightSocket();

	void UpdateRightSocket(Peer peer);

	void ListenLeftPort();

	void ListenQueryPort();

	void HandleMessage(Message message);

	void BuildDHTNetwork(std::vector<std::string> args);

	void BuildDHT();

	void RebuildDHT(Message message, std::vector<std::string> args);

	void TeardownDHT(Message message, std::vector<std::string> args);

	void StoreDHTEntry(std::vector<std::string> args);

	std::vector<std::string> GetDHTEntry(std::string longName, int pos);

	void ReturnDHTEntry(Message message, std::vector<std::string> args);

	void PrintDHTEntry(std::vector<std::string> args);

	void AddDHTPeer(std::vector<std::string> args);

	void FinishJoinDHT();

	void FinishLeaveDHT();

	void ResetDHTID(std::vector<std::string> args);

	void SetDHTPeerInfo(std::vector<std::string> args);

	void ResetDHTPeerInfo(std::vector<std::string> args);

	std::vector<std::string> SendMessageWResponse(Socket socket, std::string msg);

	std::vector<std::string> SendMessageWResponse(Socket socket, std::vector<std::string> args);

	void SendMessageNoResponse(Socket socket, std::string msg);

	void SendMessageNoResponse(Socket socket, std::vector<std::string> args);

	void RespondToMessage(Message message, std::vector<std::string> args);
};

bool ValidateIPAddress(std::string input);