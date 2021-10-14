#pragma once

#include <iostream>
#include <vector>



class Server
{
public:
	Server(int port);
	~Server();
	void StartServer();

private:
	std::string HandleMessage(std::string msg);
	std::string RegisterPeer(std::vector<std::string> args);
	std::string DeregisterPeer(std::vector<std::string> args);
	std::string StartDHTSetup(std::vector<std::string> args);
	std::string StartDHTRebuild(std::vector<std::string> args);
	std::string StartDHTTearddown(std::vector<std::string> args);
	std::string UpdateDHTStatus(std::vector<std::string> args);
	std::string AddDHTPeer(std::vector<std::string> args);
	std::string DelDHTPeer(std::vector<std::string> args);
	std::string HandleDHTQuery(std::vector<std::string> args);
};

