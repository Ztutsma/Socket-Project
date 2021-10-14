#pragma once

#include <iostream>
#include <vector>

class Client
{
public:
	Client();
	~Client();

	void StartClient();

private:
	void HandleMessage(std::string msg);

	void RequestRegister();

	void RequestDeregister();

	void RequestDHTSetup();

	void RequestJoinDHT();

	void RequestLeaveDHT();

	void RequestRebuildDHT();

	void RequestDHTTearddown();

	void BuildDHT();

	void SetDHTPeerInfo(std::vector<std::string> args);

	void QueryDHT();

};

