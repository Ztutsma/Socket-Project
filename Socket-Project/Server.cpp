#include "Server.h"

int main(int argc, char* argv[])
{
	int serverPort;

	// Set default port if one is not provided
	if (argc != 2)
	{
		serverPort = 28500;
	}
	else
	{
		int port = atoi(argv[1]);
	}

	// Start server
	Server* server = new Server(serverPort);
	server->StartServer();
	printf("server: Port server is listening to is: %d\n", serverPort);

	// Begin user input loop
	std::string userInput = "";
	std::cin.clear();
	std::cin.ignore(10000, '\n');

	while (!(strcasecmp(userInput.c_str(), "quit") == 0) || !(strcasecmp(userInput.c_str(), "exit") == 0))
	{
		// Loop until user enters "quit" or "exit"
	}

	return 0;
}

Server::Server(int port)
{
	// Build Server Socket
	serverSocket.port = port;
	serverSocket.socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	memset(&serverSocket.address, 0, sizeof(serverSocket.address));		// Zero out structure
	serverSocket.address.sin_family = AF_INET;							// Internet address family
	serverSocket.address.sin_addr.s_addr = htonl(INADDR_ANY);				// Any incoming interface
	serverSocket.address.sin_port = htons(serverSocket.port);				// Local port

	bind(serverSocket.socket, (struct sockaddr*)&serverSocket.address, sizeof(serverSocket.address));
}

Server::~Server()
{

}

void Server::StartServer()
{
	// Run threads
	threads.push_back(std::thread(&Server::ListenToPort, this));
}

void Server::ListenToPort()
{
	message.addrLen = sizeof(message.address);

	// Loop infinitely
	while (true)
	{
		// Listen to port
		message.msgSize = recvfrom(serverSocket.socket, message.buffer, BUFFERMAX, 0, (struct sockaddr*) &message.address, &message.addrLen);

		message.buffer[message.msgSize] = '\0';
		message.inMsg = std::string(message.buffer);
		
		// Handle message
		message.outMsg = const_cast<char*>(HandleMessage(message.inMsg).c_str());

		// Reply to client

		sendto(serverSocket.socket, message.outMsg, strlen(message.outMsg), 0, (struct sockaddr*)&message.address, sizeof(message.addrLen));
	}
}

std::string Server::HandleMessage(std::string msg)
{
	std::string response;

	// Check if server is accepting messages
	// Proceed if dht is not being built, rebuilt, or torn down
	if (dhtStatus == Building || dhtStatus == Rebuilding || dhtStatus == Teardown)
	{
		return "FAILURE";
	}

	// Parse message into list of arguments
	std::vector <std::string> args = ParseMessage(msg);

	// Switch based on first argument
	if (args[0] == "register")
	{
		response = RegisterPeer(args);
	}

	if (args[0] == "deregister")
	{
		response = DeregisterPeer(args);
	}

	if (args[0] == "setup-dht")
	{
		response = StartDHTSetup(args);
	}

	if (args[0] == "teardown-dht")
	{
		response = StartDHTTearddown(args);
	}

	if (args[0] == "join-dht")
	{
		response = AddDHTPeer(args);
	}

	if (args[0] == "leave-dht")
	{
		response = DelDHTPeer(args);
	}

	if (args[0] == "query-dht")
	{
		response = HandleDHTQuery(args);
	}

	if (args[0] == "dht-complete" || args[0] == "dht-rebuilt" || args[0] == "teardown-complete")
	{
		response = UpdateDHTStatus(args);
	}

	// Return response
	return response;
}

std::string Server::RegisterPeer(std::vector <std::string> args)
{
	std::string username = args[1];
	std::string IPAddress = args[2];
	std::string leftPort = args[3];
	std::string rightPort = args[4];
	std::string queryPort = args[5];


	if (peers.size() != 0)
	{
		for (Peer peer : peers)
		{
			// Check for unique username
			if (peer.uname == username)
			{
				return "FAILURE";
			}

			// Check for no IP Address and Port conflicts
			if (peer.IPAddr == IPAddress)
			{
				if (std::to_string(peer.leftPort) == leftPort ||
					std::to_string(peer.rightPort) == rightPort ||
					std::to_string(peer.queryPort) == queryPort)
				{
					return "FAILURE";
				}
			}
		}
	}

	// Check for username length
	if (username.length() > 15)
	{
		return "FAILURE";
	}
	
	// Add to list of registered peers
	Peer newPeer;
	newPeer.uname = username;
	newPeer.IPAddr = IPAddress;
	newPeer.leftPort = stoi(leftPort);
	newPeer.rightPort = stoi(rightPort);
	newPeer.queryPort = stoi(queryPort);
	newPeer.state = Free;

	peers.push_back(newPeer);

	return "SUCCESS";
}

std::string Server::DeregisterPeer(std::vector <std::string> args)
{
	std::string username = args[1];
	int peerIndex;
	Peer* peer;

	// Check if peer is registered
	peerIndex = IsRegisteredPeer(username);

	if (peerIndex == -1)
	{
		return "FAILURE";
	}

	peer = &peers[peerIndex];

	// Check if peer is in DHT
	if (peer->state != Free)
	{
		return "FAILURE";
	}

	// Remove from list of registered peers
	peers.erase(peers.begin() + peerIndex);

	return "SUCCESS";
}

std::string Server::StartDHTSetup(std::vector <std::string> args)
{
	int nodeCount = stoi(args[1]);
	std::string username = args[2];
	int peerIndex;
	Peer* leader;

	// Check if peer is registered
	peerIndex = IsRegisteredPeer(username);

	if (peerIndex == -1)
	{
		return "FAILURE";
	}

	// Check if DHT already exists
	if (dhtStatus != None)
	{
		return "FAILURE";
	}

	// Check if there are more than 2 nodes being requested
	if (nodeCount < 2)
	{
		return "FAILURE";
	}

	// Check if there are at least as many peers as nodes being requested
	if (peers.size() < nodeCount)
	{
		return "FAILURE";
	}

	// Set peer to leader
	leader = &peers[peerIndex];
	leader->state = Leader;
	this->leader = leader;

	// Begin building return message
	args.clear();
	args.push_back("SUCCESS");
	args.push_back(leader->uname);
	args.push_back(leader->IPAddr);
	args.push_back(std::to_string(leader->leftPort));
	args.push_back(std::to_string(leader->rightPort));
	args.push_back(std::to_string(leader->queryPort));

	// Select random nodes to be in DHT
	srand(time(NULL));
	int randInt;
	int peersInDHT = 0;

	while (peersInDHT < nodeCount - 1)
	{
		randInt = rand() % peers.size();

		if (peers[randInt].uname == leader->uname || peers[randInt].state == InDHT)
		{
			continue;
		}

		peers[randInt].state = InDHT;

		// Add peer to return message
		args.push_back(peers[randInt].uname);
		args.push_back(peers[randInt].IPAddr);
		args.push_back(std::to_string(peers[randInt].leftPort));
		args.push_back(std::to_string(peers[randInt].rightPort));
		args.push_back(std::to_string(peers[randInt].queryPort));

		peersInDHT++;
	}

	// Set DHT status as being built
	dhtStatus = Building;

	// Format message
	std::string returnMessage = FormatMessage(args);

	// Return message containing tuples of nodes in DHT
	return returnMessage;
}

std::string Server::StartDHTTearddown(std::vector <std::string> args)
{
	// Check if peer is registered

	// Check if DHT exists

	// Check if peer is leader of DHT

	// Set DHT states as being torn down

}

std::string Server::UpdateDHTStatus(std::vector <std::string> args)
{
	std::string command = args[0];
	std::string username = args[1];
	int peerIndex;
	Peer* peer;

	// Check if peer is registered
	peerIndex = IsRegisteredPeer(username);

	if (peerIndex == -1)
	{
		return "FAILURE";
	}

	peer = &peers[peerIndex];

	// DHT complete
	if (command == "dht-complete")
	{
		// Check if peer is leader
		if (peer != leader)
		{
			return "FAILURE";
		}
		// Mark DHT as running
		dhtStatus = Running;
	}

	// DHT Rebuilt

		// Check if peer is the one that asked to join/leave

		// Set peer as free/in DHT

		// Set new peer as leader

		// Mark DHT as running

	// DHT Tearddown

		// Check if peer is leader of DHT

		// Set all peers to free

		// Mark DHT as not existing
}

std::string Server::AddDHTPeer(std::vector <std::string> args)
{
	// Check if peer is registered

	// Check if DHT exists

	// Check if peer is in DHT

	// Save joining peer info

	// Set DHT status as being rebuilt

	// Reply with leader's info
}

std::string Server::DelDHTPeer(std::vector <std::string> args)
{
	// Check if peer is registered

	// Check if DHT exists

	// Check if peer is in DHT

	// Save leaving peer info

	// Set DHT status as being rebuilt

}

std::string Server::HandleDHTQuery(std::vector <std::string> args)
{
	// Check if peer is registered

	// Check if DHT exists

	// Check if peer is in DHT

	// Return random peer in DHT
}

int Server::IsRegisteredPeer(std::string uname) 
{
	for (int i = 0; i < peers.size(); i++)
	{
		if (peers[i].uname == uname)
		{
			return i;
		}
	}

	return -1;
}