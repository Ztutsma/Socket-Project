// Server.cpp contains the main driver and all the methods for the
// UDP Server. At runtime a user will be required to enter a valid
// port that the server will bind and listen to.
// 
// Name: Collin Amstutz
// Student ID: 1208893539
// Class: CSE 434
//

#include "Server.h"

void DieWithError(const char* errorMessage) // External error handling function
{
	perror(errorMessage);
	exit(1);
}

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
		serverPort = atoi(argv[1]);
	}

	// Start server
	Server* server = new Server(serverPort);
	server->StartServer();
	printf("server: Port server is listening to is: %d\n", serverPort);

	// Begin user input loop
	std::string userInput = "";

	while (userInput != "quit" && userInput != "exit")
	{
		// Loop until user enters "quit" or "exit"
		std::getline(std::cin, userInput);

		// Remove leading whitespace
		userInput = userInput.substr(userInput.find_first_not_of(" \n\t\r\f\v"));
	}

	return 0;
}

Server::Server(int port)
{
	// Build Server Socket
	serverSocket.port = port;
	if ((serverSocket.socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		DieWithError("server: socket() failed");

	memset(&serverSocket.address, 0, sizeof(serverSocket.address));		// Zero out structure
	serverSocket.address.sin_family = AF_INET;							// Internet address family
	serverSocket.address.sin_addr.s_addr = htonl(INADDR_ANY);				// Any incoming interface
	serverSocket.address.sin_port = htons(serverSocket.port);				// Local port

	if (bind(serverSocket.socket, (struct sockaddr*)&serverSocket.address, sizeof(serverSocket.address)) < 0)
	{
		DieWithError("server: bind() failed");
	}
}

Server::~Server()
{

}

// Begins thread that listens to Socket
void Server::StartServer()
{
	// Run threads
	threads.push_back(std::thread(&Server::ListenToPort, this));
}

void Server::ListenToPort()
{
	message.addrLen = sizeof(message.address);
	std::string response;

	// Loop infinitely
	while (true)
	{
		// Listen to port
		message.msgSize = recvfrom(serverSocket.socket, message.buffer, BUFFERMAX, 0, (struct sockaddr*) &message.address, &message.addrLen);

		message.buffer[message.msgSize] = '\0';
		message.inMsg = std::string(message.buffer);

		response = HandleMessage(message.inMsg);

		// Handle message
		message.outMsg = const_cast<char*>(response.c_str());

		// Reply to client
		sendto(serverSocket.socket, message.outMsg, strlen(message.outMsg), 0, (struct sockaddr*)&message.address, sizeof(message.address));
	}
}

// Parses the message received into a list of arguments
// then calls the method that corresponds to the first argument
//
std::string Server::HandleMessage(std::string msg)
{
	std::string response;

	// Parse message into list of arguments
	std::vector <std::string> args = ParseMessage(msg);

	// Update DHT Status to allow other commands through
	if (args[0] == "dht-complete" || args[0] == "dht-rebuilt" || args[0] == "teardown-complete")
	{
		response = UpdateDHTStatus(args);
	}

	// Check if server is accepting messages
	// Proceed if dht is not being built, rebuilt, or torn down
	if (dhtStatus == Building || dhtStatus == Rebuilding || dhtStatus == Teardown)
	{
		return "FAILURE";
	}

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
		response = StartDHTTeardown(args);
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

	printf("\nRequest to register received from %s\n", username.c_str());

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

	printf("\nRequest to register approved\n");

	return "SUCCESS";
}

std::string Server::DeregisterPeer(std::vector <std::string> args)
{
	std::string username = args[1];
	int peerIndex;
	Peer peer;

	printf("\nRequest to deregister received from %s\n", username.c_str());

	// Check if peer is registered
	peerIndex = GetPeerIndex(username);

	if (peerIndex == -1)
	{
		return "FAILURE";
	}

	peer = peers[peerIndex];

	// Check if peer is in DHT
	if (peer.state != Free)
	{
		return "FAILURE";
	}

	printf("Request to deregister approved\n");

	// Remove from list of registered peers
	peers.erase(peers.begin() + peerIndex);

	printf("%s removed from list of known peers.\n", username.c_str());

	return "SUCCESS";
}

std::string Server::StartDHTSetup(std::vector <std::string> args)
{
	int nodeCount = stoi(args[1]);
	std::string username = args[2];
	int peerIndex;

	printf("\nRequest to setup DHT received from %s\n", username.c_str());

	// Check if peer is registered
	peerIndex = GetPeerIndex(username);

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

	printf("Request to setup DHT approved\n");

	// Set peer to leader
	peers[peerIndex].state = Leader;
	this->leader = peers[peerIndex];

	printf("Leader of DHT: %s\n", this->leader.uname.c_str());

	// Begin building return message
	args.clear();
	args.push_back("SUCCESS");
	args.push_back(this->leader.uname);
	args.push_back(this->leader.IPAddr);
	args.push_back(std::to_string(this->leader.leftPort));
	args.push_back(std::to_string(this->leader.rightPort));
	args.push_back(std::to_string(this->leader.queryPort));

	// Select random nodes to be in DHT
	srand(time(NULL));
	int randInt;
	int peersInDHT = 0;

	while (peersInDHT < nodeCount - 1)
	{
		randInt = rand() % peers.size();

		if (peers[randInt].uname == this->leader.uname || peers[randInt].state == InDHT)
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

		printf("Peer chosen for DHT: %s\n", peers[randInt].uname.c_str());

		peersInDHT++;
	}

	// Set DHT Ring Size
	dhtRingSize = nodeCount;

	// Set DHT status as being built
	dhtStatus = Building;

	printf("DHT is now: Building\n");

	// Format message
	std::string returnMessage;
	returnMessage = FormatMessage(args);

	// Return message containing tuples of nodes in DHT
	return returnMessage;
}

std::string Server::StartDHTTeardown(std::vector <std::string> args)
{
	std::string username = args[1];
	int peerIndex;
	Peer peer;

	printf("\nRequest to teardown DHT received from %s\n", username.c_str());

	// Check if peer is registered
	peerIndex = GetPeerIndex(username);

	if (peerIndex == -1)
	{
		return "FAILURE";
	}

	peer = peers[peerIndex];

	// Check if DHT exists
	if (dhtStatus != Running)
	{
		return "FAILURE";
	}

	// Check if peer is leader of DHT
	if (peer.uname != leader.uname)
	{
		return "FAILURE";
	}

	printf("Request to setup DHT approved\n");

	// Set DHT states as being torn down
	dhtStatus = Teardown;

	printf("DHT is now: Tearing Down\n");

	return "SUCCESS";
}

std::string Server::AddDHTPeer(std::vector <std::string> args)
{
	std::string username = args[1];
	int peerIndex;
	Peer peer;

	printf("\nRequest to join DHT received from %s\n", username.c_str());

	// Check if peer is registered
	peerIndex = GetPeerIndex(username);

	if (peerIndex == -1)
	{
		return "FAILURE";
	}

	peer = peers[peerIndex];

	// Check if DHT exists
	if (dhtStatus != Running)
	{
		return "FAILURE";
	}

	// Check if peer is in DHT
	if (peer.state != Free)
	{
		return "FAILURE";
	}

	printf("Request to join DHT approved\n");

	// Increase ring size by 1
	dhtRingSize++;

	// Save joining peer info
	cachedPeer = peer;
	cachedPeer.dhtID = -1;

	// Set DHT status as being rebuilt
	dhtStatus = Rebuilding;
	printf("DHT is now: Rebuilding\n");

	// Reply with leader's info
	args.clear();
	args.push_back("SUCCESS");
	args.push_back(this->leader.uname);
	args.push_back(this->leader.IPAddr);
	args.push_back(std::to_string(this->leader.leftPort));
	args.push_back(std::to_string(this->leader.rightPort));
	args.push_back(std::to_string(this->leader.queryPort));

	std::string returnMessage;
	returnMessage = FormatMessage(args);
	return returnMessage;
}

std::string Server::DelDHTPeer(std::vector <std::string> args)
{
	std::string username = args[1];
	int peerIndex;
	Peer peer;

	printf("\nRequest to leave DHT received from %s\n", username.c_str());

	// Check if peer is registered
	peerIndex = GetPeerIndex(username);

	if (peerIndex == -1)
	{
		return "FAILURE";
	}

	// Check if there is only one peer in DHT
	if (dhtRingSize < 2)
	{
		return "FAILURE";
	}

	peer = peers[peerIndex];

	// Check if DHT exists
	if (dhtStatus != Running)
	{
		return "FAILURE";
	}

	// Check if peer is in DHT
	if (peer.state == Free)
	{
		return "FAILURE";
	}

	printf("Request to leave DHT approved\n");

	// Decrease Ring Size
	dhtRingSize--;

	// Save leaving peer info
	cachedPeer = peer;
	cachedPeer.dhtID = -2;

	// Set DHT status as being rebuilt
	dhtStatus = Rebuilding;
	printf("DHT is now: Rebuilding\n");

	return "SUCCESS";
}

std::string Server::UpdateDHTStatus(std::vector <std::string> args)
{
	std::string command = args[0];
	std::string username = args[1];
	int peerIndex;

	// Check if peer is registered
	peerIndex = GetPeerIndex(username);

	if (peerIndex == -1)
	{
		return "FAILURE";
	}

	printf("Updating DHT Status with... %s\n", command.c_str());

	// DHT complete
	if (command == "dht-complete")
	{
		// Check if peer is leader
		if (peers[peerIndex].uname != leader.uname)
		{
			return "FAILURE";
		}
		// Mark DHT as running
		dhtStatus = Running;
		printf("DHT is now: Running\n");

		return "SUCCESS";
	}

	// DHT Rebuilt
	if (command == "dht-rebuilt")
	{
		std::string newLeaderUname = args[2];
		int newLeaderIndex;

		// Check if peer is the one that asked to join/leave
		if (peers[peerIndex].uname != cachedPeer.uname)
		{
			return "FAILURE";
		}

		// Check if new leader is registered
		newLeaderIndex = GetPeerIndex(newLeaderUname);

		if (peerIndex == -1)
		{
			return "FAILURE";
		}

		// Set peer as free/in DHT
		if (cachedPeer.dhtID == -1)
		{
			peers[peerIndex].state = InDHT;
		}
		else
		{
			peers[peerIndex].state = Free;
		}

		// Set new peer as leader
		peers[newLeaderIndex].state = Leader;
		if (peers[newLeaderIndex].uname != leader.uname)
		{
			if (this->leader.state != Free)
			{
				this->leader.state = InDHT;
			}
			this->leader = peers[newLeaderIndex];
		}

		// Mark DHT as running
		dhtStatus = Running;
		printf("DHT is now: Running\n");
		printf("New leader is: %s", this->leader.uname.c_str());

		return "SUCCESS";
	}

	// DHT Teardown
	if (command == "teardown-complete")
	{
		// Check if peer is leader of DHT
		if (peers[peerIndex].uname != leader.uname)
		{
			return "FAILURE";
		}

		// Set all peers to free
		for (int i = 0; i < peers.size(); i++)
		{
			if (peers[peerIndex].state != Free)
			{
				peers[peerIndex].state = Free;
			}
		}

		// Mark DHT as not existing
		dhtStatus = None;
		printf("DHT is now: Torn Down\n");

		return "SUCCESS";
	}

	// Something Unexpected Happened
	return "FAILURE";
}

std::string Server::HandleDHTQuery(std::vector <std::string> args)
{
	std::string username = args[1];
	int peerIndex;
	Peer peer;

	printf("\nRequest to query DHT received from %s\n", username.c_str());

	// Check if peer is registered
	peerIndex = GetPeerIndex(username);

	if (peerIndex == -1)
	{
		return "FAILURE";
	}

	peer = peers[peerIndex];

	// Check if DHT exists
	if (dhtStatus != Running)
	{
		return "FAILURE";
	}

	// Check if peer is in DHT
	if (peer.state != Free)
	{
		return "FAILURE";
	}

	printf("Request to query DHT approved\n");

	// Select random peer in DHT
	srand(time(NULL));
	int randInt = rand() % peers.size();
	Peer tempPeer = peers[randInt];

	while (tempPeer.state == Free)
	{
		randInt = rand() % peers.size();
		tempPeer = peers[randInt];
	}

	// Format random peers info
	args.clear();
	args.push_back("SUCCESS");
	args.push_back(tempPeer.uname);
	args.push_back(tempPeer.IPAddr);
	args.push_back(std::to_string(tempPeer.queryPort));

	printf("Providing peer from DHT to %s\n", username.c_str());
	printf("Name: %s   IP Address: %s   Port: %s\n", 
		tempPeer.uname.c_str(), tempPeer.IPAddr.c_str(), std::to_string(tempPeer.queryPort).c_str());

	std::string returnMessage;
	returnMessage = FormatMessage(args);

	// Return info
	return returnMessage;
}

int Server::GetPeerIndex(std::string uname) 
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