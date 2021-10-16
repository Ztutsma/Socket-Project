#pragma once

#include "socket-helper.h"
#include "Parser.h"
#include <regex>


class Client
{
public:
	Client(std::string IPAddress, int serverPort);
	~Client();

	bool RequestRegister(std::vector<std::string> args);

	bool RequestDeregister(std::vector<std::string> args);

	bool RequestDHTSetup(std::vector<std::string> args);

	void RequestJoinDHT(std::vector<std::string> args);

	void RequestLeaveDHT(std::vector<std::string> args);

	void RequestRebuildDHT();

	void RequestDHTTearddown();

	bool RequestQueryDHT(std::vector<std::string> args);

	void QueryDHT(std::vector<std::string> args);

private:
	Socket serverSocket;
	Peer self;

	void StartClient();

	void BuildLeftSocket();

	void BuildRightSocket();

	void BuildQuerySocket();

	void ListenLeftPort();

	void ListenQueryPort();

	void HandleMessage(std::string msg);

	void BuildDHTNetwork();

	void BuildDHT();

	void RebuildDHT();

	void SetDHTPeerInfo(std::vector<std::string> args);

	void StoreDHTEntry(std::vector<std::string> args);

	std::vector<std::string> SendMessage(Socket socket, std::string msg);

	std::vector<std::string> SendMessage(Socket socket, std::vector<std::string> args);
};

bool ValidateIPAddress(std::string input);