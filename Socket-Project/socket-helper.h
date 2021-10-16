#pragma once
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <thread>
#include <time.h>

#define BUFFERMAX 1000

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
	int dhtID;
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

DHTStatus dhtStatus = None;