// Socket-Project.cpp : Defines the entry point for the application.
//

#include "Client.h"

#define DEBUG

void DieWithError(const char* errorMessage) // External error handling function
{
	perror(errorMessage);
	exit(1);
}

int main(int argc, char* argv[])
{
	std::string userInput = "";

	// Set Server Port & IP Address
	std::string serverIP;
	int serverPort = 0;
	bool servIPValid = false;

#ifdef DEBUG
	serverPort = 28500;
	serverIP = "172.31.28.134";
#endif // CJDEBUG


	if (argc == 3)
	{
		serverIP = argv[1];
		serverPort = atoi(argv[2]);
	}

	// Validate Server Port & IP Address
	servIPValid = ValidateIPAddress(serverIP);

	while (!servIPValid)
	{
		std::cout << "Please enter a valid IPV4 address for the server: ";
		std::getline(std::cin, userInput);
		serverIP = userInput;

		servIPValid = ValidateIPAddress(serverIP);
	}

	while (serverPort < 28500 || serverPort > 28999)
	{
		std::cout << "Please enter a valid port number for the server (28500-28999): ";
		std::getline(std::cin, userInput);
		serverPort = stoi(userInput);
	}

	printf("Server IP address: %s\n", serverIP.c_str());
	printf("Server port: %d\n", serverPort);


	// Create Client
	Client* client = new Client(serverIP, serverPort);


	// Begin client user input loop
	std::vector<std::string> args;

	while (userInput != "quit" && userInput != "exit")
	{
		std::cout << "IN >> ";
		std::getline(std::cin, userInput);

		// Remove leading whitespace
		userInput = userInput.substr(userInput.find_first_not_of(" \n\t\r\f\v"));

		// Parse user input into list of arguments
		args = ParseUInput(userInput);

		// Switch off first argument
		// Register Peer
		if (args[0] == "register")
		{
			if (args.size() != 6)
			{
				printf("Incorrect format used.\n");
				printf("Correct format is: register 'username' 'IPaddress' 'Port#' 'Port#' 'Port#'\n");
				continue;
			}
			if (client->RequestRegister(args))
			{
				printf("Registration successful\n");
			}
			else
			{
				printf("Registration failed\n");
			}
		}
		
		// Deregister Peer
		if (args[0] == "deregister")
		{
			if (args.size() < 2)
			{
				printf("Incorrect format used.\n");
				printf("Correct format is: deregister 'username'\n");
				continue;
			}
			if (client->RequestDeregister(args))
			{
				printf("Deregistration successful\n");
			}
			else
			{
				printf("Deregistration failed\n");
			}
		}

		// Setup DHT
		if (args[0] == "setup-dht")
		{
			if (args.size() < 3)
			{
				printf("Incorrect format used.\n");
				printf("Correct format is: setup-dht 'NodeCount' 'username'\n");
				continue;
			}
			if (client->RequestDHTSetup(args))
			{
				printf("DHT Setup successful\n");
			}
			else
			{
				printf("DHT Setup failed\n");
			}
		}

		// Join DHT
		if (args[0] == "join-dht")
		{
			if (args.size() < 2)
			{
				printf("Incorrect format used.\n");
				printf("Correct format is: join-dht 'username'\n");
				continue;
			}

			if (client->RequestJoinDHT(args))
			{
				printf("Join DHT successful\n");
			}
			else
			{
				printf("Join DHT failed\n");
			}
		}

		// Leave DHT
		if (args[0] == "leave-dht")
		{
			if (args.size() < 2)
			{
				printf("Incorrect format used.\n");
				printf("Correct format is: leave-dht 'username'\n");
				continue;
			}

			if (client->RequestLeaveDHT(args))
			{
				printf("Left DHT successful\n");
			}
			else
			{
				printf("Leave DHT failed\n");
			}
		}

		// Query DHT
		if (args[0] == "query-dht")
		{
			if (args.size() < 2)
			{
				printf("Incorrect format used.\n");
				printf("Correct format is: query-dht 'username'\n");
				continue;
			}

			if (client->RequestQueryDHT(args))
			{
				printf("Connect to DHT successful\n");
				printf("To query enter: query 'Long Name'\n");
			}
			else
			{
				printf("Query DHT failed\n");
			}
		}

		if (args[0] == "query")
		{
			if (args.size() < 2)
			{
				printf("Incorrect format used.\n");
				printf("Correct format is: query 'Long Name'\n");
				continue;
			}

			client->QueryDHT(args);
		}

		// Teardown DHT
		if (args[0] == "teardown-dht")
		{
			if (args.size() < 2)
			{
				printf("Incorrect format used.\n");
				printf("Correct format is: teardown-dht 'username'\n");
				continue;
			}

			if (client->RequestDHTTearddown(args))
			{
				printf("Teardown DHT successful\n");
			}
			else
			{
				printf("Teardown DHT failed\n");
			}
		}
	}
	return 0;
}

bool ValidateIPAddress(std::string input) {
	std::string segment;
	std::vector<std::string> seglist;
	std::regex IPRegex("\\d{1,3}.\\d{1,3}.\\d{1,3}.\\d{1,3}");

	if (!std::regex_match(input, IPRegex))
	{
		return false;
	}

	std::stringstream stream(input);
	while (std::getline(stream, segment, '.'))
	{
		seglist.push_back(segment);
	}

	for (std::string var : seglist)
	{
		if (std::stoi(var) > 255 || std::stoi(var) < 0) {
			return false;
		}
	}

	return true;
}

Client::Client(std::string IPAddress, int serverPort)
{
	// Build Server Socket
	serverSocket.port = serverPort;
	serverSocket.socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	memset(&serverSocket.address, 0, sizeof(serverSocket.address));
	serverSocket.address.sin_family = AF_INET;
	serverSocket.address.sin_addr.s_addr = inet_addr(IPAddress.c_str());
	serverSocket.address.sin_port = htons(serverSocket.port);

	// Initialize Hash Table
	hashTable.assign(353, NULL);
}

Client::~Client()
{
}

// Sends request to register as a Peer with the server
bool Client::RequestRegister(std::vector<std::string> args)
{
	// Save argument info
	std::string username = args[1];
	std::string IPAddress = args[2];
	int leftPort = stoi(args[3]);
	int rightPort = stoi(args[4]);
	int queryPort = stoi(args[5]);

	// Validate Client Ports
	std::string userInput = "";
	while (leftPort < 28500 || leftPort > 28999)
	{
		std::cout << "Please enter a valid port number for the left port (28500-28999): ";
		std::getline(std::cin, userInput);
		leftPort = stoi(userInput);
	}
	if (leftPort != stoi(args[3]))
	{
		args[3] = std::to_string(leftPort);
	}

	while (rightPort < 28500 || rightPort > 28999)
	{
		std::cout << "Please enter a valid port number for the right port (28500-28999): ";
		std::getline(std::cin, userInput);
		rightPort = stoi(userInput);
	}
	if (rightPort != stoi(args[4]))
	{
		args[4] = std::to_string(rightPort);
	}

	while (queryPort < 28500 || queryPort > 28999)
	{
		std::cout << "Please enter a valid port number for the query port (28500-28999): ";
		std::getline(std::cin, userInput);
		queryPort = stoi(userInput);
	}
	if (queryPort != stoi(args[5]))
	{
		args[5] = std::to_string(queryPort);
	}

	// Message server
	args = SendMessageWResponse(serverSocket, args);

	// Check if successful
	if (args[0] != "SUCCESS")
	{
		return false;
	}

	// Set Self's Username, IP Address, & Ports
	self.uname = username;
	self.IPAddr = IPAddress;
	self.leftPort = leftPort;
	self.rightPort = rightPort;
	self.queryPort = queryPort;

	// Start Client threads
	StartClient();

	return true;
}

// Sends a request to deregister as a Peer witht he server
bool Client::RequestDeregister(std::vector<std::string> args)
{
	// Message server
	args = SendMessageWResponse(serverSocket, args);

	// Check if successful
	if (args[0] != "SUCCESS")
	{
		return false;
	}

	// Delete references to self
	self.uname = "";
	self.IPAddr = "";
	self.leftPort = 0;
	self.rightPort = 0;
	self.queryPort = 0;

	// Close sockets
	close(leftSocket.socket);
	close(rightSocket.socket);
	close(querySocket.socket);

	return true;
}

// Sends a request to create the DHT
bool Client::RequestDHTSetup(std::vector<std::string> args)
{
	// Message server
	args = SendMessageWResponse(serverSocket, args);

	// Check if successful
	if (args[0] != "SUCCESS")
	{
		return false;
	}

	// Build DHT Network
	BuildDHTNetwork(args);

	// Build DHT
	BuildDHT();

	// Tell Server DHT is complete
	args.clear();
	args.push_back("dht-complete");
	args.push_back(self.uname);
	args = SendMessageWResponse(serverSocket, args);
	
	if (args[0] != "SUCCESS")
	{
		return false;
	}

	return true;
}

bool Client::RequestJoinDHT(std::vector<std::string> args)
{
	// Message server
	args = SendMessageWResponse(serverSocket, args);

	// Check if successful
	if (args[0] != "SUCCESS")
	{
		return false;
	}

	// Set id to -1 to remember self is requesting to join
	self.dhtID = -1;

	// Set Leader as Right peer
	int i = 1;

	rightPeer.uname = args[i++];
	rightPeer.IPAddr = args[i++];
	rightPeer.leftPort = stoi(args[i++]);
	rightPeer.rightPort = stoi(args[i++]);
	rightPeer.queryPort = stoi(args[i++]);

	UpdateRightSocket(rightPeer);

	// Begin Join DHT Process

	// Tell leader to add DHT peer
	args.clear();
	args.push_back("add-peer");
	args.push_back(self.uname);
	args.push_back(self.IPAddr);
	args.push_back(std::to_string(self.leftPort));
	args.push_back(std::to_string(self.rightPort));
	args.push_back(std::to_string(self.queryPort));

	SendMessageNoResponse(rightSocket, args);

	// Lock thread until JoinDHT() steps have been completed
	joinedDht = false;
	while (!joinedDht);
	// Thread is unlocked by JoinDHT()
	joinedDht = false; // Reset Lock

	// Tell server dht is rebuilt
	args.clear();
	args.push_back("dht-rebuilt");
	args.push_back(self.uname);
	args.push_back(rightPeer.uname);

	args = SendMessageWResponse(serverSocket, args);

	if (args[0] != "SUCCESS")
	{
		return false;
	}

	return true;
}

bool Client::RequestLeaveDHT(std::vector<std::string> args)
{
	args = SendMessageWResponse(serverSocket, args);

	// Check if successful
	if (args[0] != "SUCCESS")
	{
		return false;
	}

	// Set ID to -1 to note node is leaving
	self.dhtID = -1;

	// Set dht status to teardown to note self was the one that requested teardown
	dhtStatus = Teardown;

	// Begin Exit DHT Process

	// Send teardown to right peer
	args.clear();
	args.push_back("teardown");

	SendMessageNoResponse(rightSocket, args);
	
	// Lock thread until ExitDHT() steps have been completed
	leftDht = false;
	while (!leftDht);
	// Thread is unlocked by ExitDHT()
	leftDht = false; // Reset Lock

	// Tell server dht is rebuilt
	args.clear();
	args.push_back("dht-rebuilt");
	args.push_back(self.uname);
	args.push_back(rightPeer.uname);

	args = SendMessageWResponse(serverSocket, args);

	if (args[0] != "SUCCESS")
	{
		return false;
	}

	return true;
}

bool Client::RequestRebuildDHT()
{

}

bool Client::RequestDHTTearddown(std::vector<std::string> args)
{

#ifdef DEBUG
	printf("RTD0: %s", args[0].c_str());
#endif // CJDEBUG
	// Message server
	args = SendMessageWResponse(serverSocket, args);

#ifdef DEBUG
	printf("RTD1: %s", args[0].c_str());
#endif // CJDEBUG


	// Check if successful
	if (args[0] != "SUCCESS")
	{
		return false;
	}

	// Note dht is being torndown w/o rebuild
	dhtStatus = Teardown;
	killDHT = true;

	// Begin DHT Teardown
	
	// Send teardown to right peer
	args.clear();
	args.push_back("teardown");

	SendMessageNoResponse(rightSocket, args);

	// Lock thread until ExitDHT() steps have been completed
	dhtDead = false;
	while (!dhtDead);
	// Thread is unlocked by ExitDHT()

	// Reset Locks
	dhtDead = false; 
	killDHT = false;

	// Tell server dht is torn down
	args.clear();
	args.push_back("teardown-complete");
	args.push_back(self.uname);

	args = SendMessageWResponse(serverSocket, args);

#ifdef DEBUG
	printf("RTD2: %s", args[0].c_str());
#endif // CJDEBUG

	if (args[0] != "SUCCESS")
	{
		return false;
	}

	return true;
}

bool Client::RequestQueryDHT(std::vector<std::string> args)
{
	// Message server
	args = SendMessageWResponse(serverSocket, args);

	// Check if successful
	if (args[0] != "SUCCESS")
	{
		return false;
	}

	// Set right peer to dht peer
	int i = 1;

	rightPeer.uname = args[i++];
	rightPeer.IPAddr = args[i++];
	rightPeer.queryPort = stoi(args[i++]);

	// Queries are treated like right peers where the "left port"
	// is the query port
	rightPeer.leftPort = rightPeer.queryPort;
	UpdateRightSocket(rightPeer);

	return true;
}

void Client::QueryDHT(std::vector<std::string> args)
{
	// Format Message
	args = FormatQuery(args);

	// Message DHT member
	args = SendMessageWResponse(rightSocket, args);

	// Check if response was received
	if (args[0] != "SUCCESS" || args.size() != 10)
	{
		printf("Query Failed\n");
		return;
	}

	// Print Response
	PrintDHTEntry(args);

}

void Client::StartClient()
{
	// Build Sockets
	BuildLeftSocket();
	BuildQuerySocket();
	BuildRightSocket();

	// Start Listening Threads
	threads.push_back(std::thread(&Client::ListenLeftPort, this));
	threads.push_back(std::thread(&Client::ListenQueryPort, this));
}

void Client::BuildLeftSocket()
{
	leftSocket.port = self.leftPort;
	if ((leftSocket.socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		DieWithError("server: socket() failed");

	memset(&leftSocket.address, 0, sizeof(leftSocket.address));		// Zero out structure
	leftSocket.address.sin_family = AF_INET;							// Internet address family
	leftSocket.address.sin_addr.s_addr = htonl(INADDR_ANY);				// Any incoming interface
	leftSocket.address.sin_port = htons(leftSocket.port);				// Local port

	if (bind(leftSocket.socket, (struct sockaddr*)&leftSocket.address, sizeof(leftSocket.address)) < 0)
	{
		DieWithError("server: bind() failed");
	}
}

void Client::BuildQuerySocket()
{
	querySocket.port = self.queryPort;
	if ((querySocket.socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		DieWithError("server: socket() failed");

	memset(&querySocket.address, 0, sizeof(querySocket.address));		// Zero out structure
	querySocket.address.sin_family = AF_INET;							// Internet address family
	querySocket.address.sin_addr.s_addr = htonl(INADDR_ANY);				// Any incoming interface
	querySocket.address.sin_port = htons(querySocket.port);				// Local port

	if (bind(querySocket.socket, (struct sockaddr*)&querySocket.address, sizeof(querySocket.address)) < 0)
	{
		DieWithError("server: bind() failed");
	}
}

void Client::BuildRightSocket()
{
	if ((rightSocket.socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		DieWithError("server: socket() failed");

	memset(&rightSocket.address, 0, sizeof(rightSocket.address));
	rightSocket.address.sin_family = AF_INET;
}

void Client::UpdateRightSocket(Peer peer)
{
	rightSocket.port = peer.leftPort;
	rightSocket.address.sin_addr.s_addr = inet_addr(peer.IPAddr.c_str());
	rightSocket.address.sin_port = htons(rightSocket.port);
}

void Client::ListenLeftPort()
{
	Message message;

	message.addrLen = sizeof(message.address);

	// Loop infinitely
	while (true)
	{
		// Clean buffer
		memset(message.buffer, 0, sizeof(message.buffer));

		// Listen to port
		message.msgSize = recvfrom(leftSocket.socket, message.buffer, BUFFERMAX, 0, (struct sockaddr*)&message.address, &message.addrLen);

		message.buffer[message.msgSize] = '\0';
		message.inMsg = std::string(message.buffer);

		printf("\nLPort: %s\n", message.inMsg.c_str());

		// Handle message
		HandleMessage(message);
	}
}

void Client::ListenQueryPort()
{
	Message message;

	message.addrLen = sizeof(message.address);

	// Loop infinitely
	while (true)
	{
		// Clean buffer
		memset(message.buffer, 0, sizeof(message.buffer));

		// Listen to port
		message.msgSize = recvfrom(querySocket.socket, message.buffer, BUFFERMAX, 0, (struct sockaddr*)&message.address, &message.addrLen);

		message.buffer[message.msgSize] = '\0';
		message.inMsg = std::string(message.buffer);

		printf("\nQPort: %s\n", message.inMsg.c_str());

		// Handle message
		HandleMessage(message);
	}
}

void Client::HandleMessage(Message message)
{
	// Parse message
	std::string msg = message.inMsg;
	std::vector<std::string> args = ParseMessage(msg);

	// Switch based off first argument
	if (args[0] == "set-id")
	{
		SetDHTPeerInfo(args);
	}
	if (args[0] == "store")
	{
		StoreDHTEntry(args);
	}
	if (args[0] == "teardown")
	{
		TeardownDHT(message, args);
	}
	if (args[0] == "add-peer")
	{
		AddDHTPeer(args);
	}
	if (args[0] == "reset-id")
	{
		ResetDHTID(args);
	}
	if (args[0] == "rebuild-dht")
	{
		RebuildDHT(message, args);
	}
	if (args[0] == "reset-left" || args[0] == "reset-right")
	{
		ResetDHTPeerInfo(args);
	}
	if (args[0] == "query")
	{
		ReturnDHTEntry(message, args);
	}
}

void Client::BuildDHTNetwork(std::vector<std::string> args)
{
	// Add peers to list of peers in dht
	Peer peer;
	for (int i = 1; i < args.size();)
	{
		peer.uname = args[i++];
		peer.IPAddr = args[i++];
		peer.leftPort = stoi(args[i++]);
		peer.rightPort = stoi(args[i++]);
		peer.queryPort = stoi(args[i++]);

		dhtPeers.push_back(peer);
	}

	// Tell peers in network to set IDs and neighbours
	int leftPeerIndex, rightPeerIndex;
	int ringSize = dhtPeers.size();
	for (int i = 1; i < ringSize; i++)
	{
		leftPeerIndex = i - 1;
		rightPeerIndex = (i + 1) % ringSize;
		args.clear();
		args.push_back("set-id");
		args.push_back(std::to_string(i));									// DHT ID of node i
		args.push_back(std::to_string(ringSize));							// Ring size of DHT
		args.push_back(dhtPeers[leftPeerIndex].uname);						// Left peer of node i
		args.push_back(dhtPeers[leftPeerIndex].IPAddr);						// IP Address of node's left peer
		args.push_back(std::to_string(dhtPeers[leftPeerIndex].leftPort));	// Left peer's left port
		args.push_back(std::to_string(dhtPeers[leftPeerIndex].rightPort));	// Left peer's right port
		args.push_back(std::to_string(dhtPeers[leftPeerIndex].queryPort));	// Left peer's query port

		args.push_back(dhtPeers[rightPeerIndex].uname);						// Right peer of node i
		args.push_back(dhtPeers[rightPeerIndex].IPAddr);					// IP Address of node's right peer
		args.push_back(std::to_string(dhtPeers[rightPeerIndex].leftPort));	// Right peer's left port
		args.push_back(std::to_string(dhtPeers[rightPeerIndex].rightPort));	// Right peer's right port
		args.push_back(std::to_string(dhtPeers[rightPeerIndex].queryPort));	// Left peer's query port

		// Temporarily set right peer to target node and update socket
		rightPeer = dhtPeers[i];
		UpdateRightSocket(rightPeer);

		// Send message to peer
		SendMessageNoResponse(rightSocket, args);
	}

	// Set DHT Peer info of leader
	leftPeerIndex = ringSize - 1;
	rightPeerIndex = 1;

	self.dhtID = 0;
	self.state = Leader;
	dhtRingSize = ringSize;
	leftPeer = dhtPeers[leftPeerIndex];
	rightPeer = dhtPeers[rightPeerIndex];

	// Update right socket of leader to "permanent" right peer
	UpdateRightSocket(rightPeer);
}

void Client::BuildDHT()
{
	std::ifstream inputFile("StatsCountry.csv");
	std::string line;

	HashNode node;

	std::vector<std::string> args;

	int pos;
	int id;

#ifdef DEBUG
	printf("Before Reading File");
#endif // DEBUG


	// Discard first line of file
	std::getline(inputFile, line);

	// Loop
	while (!inputFile.eof())
	{
		args.clear();
		pos = 0;
		id = -1;

		// Parse CSV data entry
		std::getline(inputFile, line);

		std::stringstream stream(line);
		std::getline(stream, node.countryCode, ',');
		std::getline(stream, node.shortName, ',');
		std::getline(stream, node.tableName, ',');
		std::getline(stream, node.longName, ',');
		std::getline(stream, node.alphaCode, ',');
		std::getline(stream, node.currencyUnit, ',');
		std::getline(stream, node.region, ',');
		std::getline(stream, node.wb2Code, ',');
		std::getline(stream, node.lastPopCensus, '\n');

		// Get hashed values from long name
		for (char c : node.longName) {
			pos += c;
		}
		pos = pos % 353;
		id = pos % dhtRingSize;

		// Store DHT Entry
		args.push_back("store");
		args.push_back(std::to_string(id));
		args.push_back(std::to_string(pos));
		args.push_back(node.countryCode);
		args.push_back(node.shortName);
		args.push_back(node.tableName);
		args.push_back(node.longName);
		args.push_back(node.alphaCode);
		args.push_back(node.currencyUnit);
		args.push_back(node.region);
		args.push_back(node.wb2Code);
		args.push_back(node.lastPopCensus);

		StoreDHTEntry(args);
	}

#ifdef DEBUG
	printf("After Reading File");
#endif // DEBUG
}

void Client::JoinDHT()
{
	std::vector<std::string> args;

	// Tell left peer to reset right
	args.push_back("reset-right");
	args.push_back(self.uname);
	args.push_back(self.IPAddr);
	args.push_back(std::to_string(self.leftPort));
	args.push_back(std::to_string(self.rightPort));
	args.push_back(std::to_string(self.queryPort));

	UpdateRightSocket(leftPeer);
	SendMessageNoResponse(rightSocket, args);
	UpdateRightSocket(rightPeer);

	// Tell right peer to reset left
	args[0] = "reset-left";
	
	SendMessageNoResponse(rightSocket, args);
	
	// Tell right peer to reset IDs
	args.clear();
	args.push_back("reset-id");
	args.push_back("0");

	SendMessageNoResponse(rightSocket, args);

	// Tell right peer to rebuild dht
	args.clear();
	args.push_back("rebuild-dht");

	// Wait for response
	args = SendMessageWResponse(rightSocket, args);

	if (args[0] == "dht-rebuilt")
	{
		joinedDht = true;
	}
}

void Client::ExitDHT()
{
	std::vector<std::string> args;

	// Tell left peer to reset right
	args.push_back("reset-right");
	args.push_back(rightPeer.uname);
	args.push_back(rightPeer.IPAddr);
	args.push_back(std::to_string(rightPeer.leftPort));
	args.push_back(std::to_string(rightPeer.rightPort));
	args.push_back(std::to_string(rightPeer.queryPort));
	
	UpdateRightSocket(leftPeer);
	SendMessageNoResponse(rightSocket, args);
	UpdateRightSocket(rightPeer);

	// Tell right peer to reset left
	args.clear();
	args.push_back("reset-left");
	args.push_back(leftPeer.uname);
	args.push_back(leftPeer.IPAddr);
	args.push_back(std::to_string(leftPeer.leftPort));
	args.push_back(std::to_string(leftPeer.rightPort));
	args.push_back(std::to_string(leftPeer.queryPort));

	SendMessageNoResponse(rightSocket, args);

	// Tell right peer to rebuild dht
	args.clear();
	args.push_back("rebuild-dht");

	// Wait for response
	args = SendMessageWResponse(rightSocket, args);

	if (args[0] == "dht-rebuilt")
	{
		leftDht = true;
	}
}

void Client::AddDHTPeer(std::vector<std::string> args)
{
	int i = 1;

	this->newPeer.uname = args[i++];
	this->newPeer.IPAddr = args[i++];
	this->newPeer.leftPort = stoi(args[i++]);
	this->newPeer.rightPort = stoi(args[i++]);
	this->newPeer.queryPort = stoi(args[i++]);

	// begin dht teardown
	args.clear();
	args.push_back("teardown");

	// Note DHT is being torn down
	dhtStatus = Teardown;

	// Wait for propogation
	SendMessageNoResponse(rightSocket, args);

}

void Client::TeardownDHT(Message message, std::vector<std::string> args)
{
	// Tear down DHT
	HashNode* tempNode;

	for (int i = 0; i < hashTable.size(); i++)
	{
		while (hashTable[i] != NULL)
		{
			tempNode = hashTable[i];
			hashTable[i] = hashTable[i]->next;
			delete(tempNode);
		}
	}

	// Check if self requested teardown
	if (dhtStatus == Teardown)
	{
		// Check if dht is being completely torn down
		if (killDHT)
		{
			dhtDead = true;

			return;
		}

		// Check if self is leaving DHT
		if (self.dhtID == -1)
		{
			// Reduce ring size
			dhtRingSize--;

			// Tell right peer to reset id to 0
			args.clear();
			args.push_back("reset-id");
			args.push_back(std::to_string(0));
			args.push_back(std::to_string(dhtRingSize));

			SendMessageNoResponse(rightSocket, args);

			dhtStatus = None;

			return;
		}

		// If self is not leaving, new Peer has joined
		// Increase ring size
		dhtRingSize++;

		// Tell new peer to reset left to self's left
		args.clear();
		args.push_back("reset-left");
		args.push_back(leftPeer.uname);
		args.push_back(leftPeer.IPAddr);
		args.push_back(std::to_string(leftPeer.leftPort));
		args.push_back(std::to_string(leftPeer.rightPort));
		args.push_back(std::to_string(leftPeer.queryPort));

		UpdateRightSocket(newPeer);
		SendMessageNoResponse(rightSocket, args);
		UpdateRightSocket(rightPeer);

		//RespondToMessage(message, args);

		dhtStatus = None;

		return;
	}

	// Tell next peer in ring to teardown
	SendMessageNoResponse(rightSocket, args);
	return;
}

void Client::RebuildDHT(Message message, std::vector<std::string> args)
{

#ifdef DEBUG
	printf("Rebuild: Before Build");
#endif // DEBUG

	// Build DHT
	BuildDHT();

#ifdef DEBUG
	printf("Rebuild: After Build");
#endif // DEBUG

	// Reply that DHT is rebuilt
	args.clear();
	args.push_back("dht-rebuilt");

	RespondToMessage(message, args);
}

void Client::ResetDHTID(std::vector<std::string> args)
{
	// Check if self is leaving DHT
	if (self.dhtID == -1)
	{
		ExitDHT();
		return;
	}

	int newID = stoi(args[1]);

	// If ring size was not received, use known ring size
	int newRingSize;
	if (args.size() < 3)
	{
		newRingSize = this->dhtRingSize;
		args.push_back(std::to_string(newRingSize));
	}
	else
	{
		newRingSize = stoi(args[2]);
	}

	// return if reset-ID has propogated back to leader
	if (newID == newRingSize)
	{
		return;
	}

	this->self.dhtID = newID;
	this->dhtRingSize = newRingSize;

	// Send reset-ID command to right peer
	newID++;
	args[1] = std::to_string(newID);
	
	SendMessageNoResponse(rightSocket, args);
}

void Client::SetDHTPeerInfo(std::vector<std::string> args)
{
	int i = 1;
	// Set self ID/DHT Info
	self.dhtID = stoi(args[i++]);
	dhtRingSize = stoi(args[i++]);
	self.state = InDHT;

	// Set Left Neighbour IP Addr/Port
	leftPeer.uname = args[i++];
	leftPeer.IPAddr = args[i++];
	leftPeer.leftPort = stoi(args[i++]);
	leftPeer.rightPort = stoi(args[i++]);
	leftPeer.queryPort = stoi(args[i++]);

	// Set Right Neighbour IP Addr/Port
	rightPeer.uname = args[i++];
	rightPeer.IPAddr = args[i++];
	rightPeer.leftPort = stoi(args[i++]);
	rightPeer.rightPort = stoi(args[i++]);
	rightPeer.queryPort = stoi(args[i++]);

	// Update right socket to new right peer
	UpdateRightSocket(rightPeer);
}

void Client::ResetDHTPeerInfo(std::vector<std::string> args)
{
	Peer* resetPeer;
	if (args[0] == "reset-left")
	{
		resetPeer = &leftPeer;
	}
	if (args[0] == "reset-right")
	{
		resetPeer = &rightPeer;
	}
	int i = 1;

	resetPeer->uname = args[i++];
	resetPeer->IPAddr = args[i++];
	resetPeer->leftPort = stoi(args[i++]);
	resetPeer->rightPort = stoi(args[i++]);
	resetPeer->queryPort = stoi(args[i++]);

	if (resetPeer == &rightPeer)
	{
		UpdateRightSocket(rightPeer);
	}

	// If self is the one that initiated AddDHTPeer
	if (self.dhtID == -1)
	{
		// Temporary id
		self.dhtID = 0;

		JoinDHT();
	}

}

void Client::StoreDHTEntry(std::vector<std::string> args)
{
	// Check if entry belongs to self
	if (stoi(args[1]) == self.dhtID)
	{
		// Add entry to DHT
		int i = 3;
		HashNode* newNode = new HashNode;
		newNode->countryCode = args[i++];
		newNode->shortName = args[i++];
		newNode->tableName = args[i++];
		newNode->longName = args[i++];
		newNode->alphaCode = args[i++];
		newNode->currencyUnit = args[i++];
		newNode->region = args[i++];
		newNode->wb2Code = args[i++];
		newNode->lastPopCensus = args[i++];
		newNode->next = NULL;

		int pos = stoi(args[2]);

		// If hashtable position is empty, set new node
		if (hashTable[pos] == NULL)
		{
			hashTable[pos] = newNode;
			return;
		}

		// Otherwise add to end of hashtable position
		HashNode* node = hashTable[pos];

		while (node->next != NULL)
		{
			node = node->next;
		}

		node->next = newNode;

		return;
	}

	// Else
	// Send through DHT network
	SendMessageNoResponse(rightSocket, args);
}

std::vector<std::string> Client::GetDHTEntry(std::string longName, int pos)
{
	std::vector<std::string> response;

	HashNode* node = hashTable[pos];

	while (node != NULL && node->longName != longName)
	{
		node = node->next;
	}

	// If node with longname wasnt found
	if (node == NULL)
	{
		response.push_back("FAILURE");
		return response;
	}

	response.push_back("SUCCESS");
	response.push_back(node->countryCode);
	response.push_back(node->shortName);
	response.push_back(node->tableName);
	response.push_back(node->longName);
	response.push_back(node->alphaCode);
	response.push_back(node->currencyUnit);
	response.push_back(node->region);
	response.push_back(node->wb2Code);
	response.push_back(node->lastPopCensus);

	return response;
}

void Client::ReturnDHTEntry(Message message, std::vector<std::string> args)
{
	int pos;
	int id;
	std::string longName = args[1];

	// If position and ID have not been calculated yet
	if (args.size() == 2)
	{
		// Get hashed values from long name
		for (char c : longName) {
			pos += c;
		}
		pos = pos % 353;
		id = pos % dhtRingSize;
		args.push_back(std::to_string(id));
		args.push_back(std::to_string(pos));
	}
	else
	{
		id = stoi(args[2]);
		pos = stoi(args[3]);
	}

	// If entry is stored in self
	if (id == self.dhtID)
	{
		args = GetDHTEntry(longName, pos);
		RespondToMessage(message, args);
		return;
	}

	// Else pass through to right peer
	args = SendMessageWResponse(rightSocket, args);
	RespondToMessage(message, args);

}

void Client::PrintDHTEntry(std::vector<std::string> args)
{
	int i = 1;

	printf("\nCountry Code:\t%s", args[i++].c_str());
	printf("\nShort Name:\t\t%s", args[i++].c_str());
	printf("\nTable Name:\t\t%s", args[i++].c_str());
	printf("\nLong Name:\t\t%s", args[i++].c_str());
	printf("\nAlpha Code:\t\t%s", args[i++].c_str());
	printf("\nCurrency Unit:\t%s", args[i++].c_str());
	printf("\nRegion:\t\t\t%s", args[i++].c_str());
	printf("\nWB2 Code:\t\t%s", args[i++].c_str());
	printf("\nLast Census:\t%s\n", args[i++].c_str());
}

std::vector<std::string> Client::SendMessageWResponse(Socket socket, std::string msg)
{
	// Build Message
	Message message;
	message.addrLen = sizeof(message.address);
	message.address = socket.address;
	message.outMsg = const_cast<char*>(msg.c_str());

	// Send message to socket
	sendto(socket.socket, message.outMsg, strlen(message.outMsg), 0, (struct sockaddr*)&message.address, sizeof(message.address));

	// Wait for Response
	message.msgSize = recvfrom(socket.socket, message.buffer, BUFFERMAX, 0, (struct sockaddr*)&message.address, &message.addrLen);
	message.buffer[message.msgSize] = '\0';
	message.inMsg = std::string(message.buffer);

	// Parse response
	std::vector<std::string> args;
	args = ParseMessage(message.inMsg);

	// Return response as a list of strings
	return args;
}

std::vector<std::string> Client::SendMessageWResponse(Socket socket, std::vector<std::string> args)
{
	// Format Message
	std::string outMessage = FormatMessage(args);

	// Send formatted message
	args.clear();
	args = SendMessageWResponse(socket, outMessage);

	// Return response as a list of strings
	return args;
}

void Client::SendMessageNoResponse(Socket socket, std::string msg)
{
	// Build Message
	Message message;
	message.addrLen = sizeof(message.address);
	message.address = socket.address;
	message.outMsg = const_cast<char*>(msg.c_str());

	// Send message to socket
	sendto(socket.socket, message.outMsg, strlen(message.outMsg), 0, (struct sockaddr*)&message.address, sizeof(message.address));
}

void Client::SendMessageNoResponse(Socket socket, std::vector<std::string> args) {
	// Format Message
	std::string outMessage = FormatMessage(args);
	
	// Send formatted message
	SendMessageNoResponse(socket, outMessage);
}

void Client::RespondToMessage(Message message, std::vector<std::string> args)
{
	Socket tempSocket;
	tempSocket.address = leftSocket.address;
	leftSocket.address = message.address;
	SendMessageNoResponse(leftSocket, args);
	leftSocket.address = tempSocket.address;
}