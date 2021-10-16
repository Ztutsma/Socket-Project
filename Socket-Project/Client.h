#pragma once

#include "socket-helper.h"
#include "Parser.h"

class Client
{
public:
	Client();
	~Client();

	bool RequestRegister(std::vector<std::string> args);

	bool RequestDeregister();

	bool RequestDHTSetup();

	void RequestJoinDHT();

	void RequestLeaveDHT();

	void RequestRebuildDHT();

	void RequestDHTTearddown();

	bool RequestQueryDHT();

	void QueryDHT(std::vector<std::string> args);

private:
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

};

