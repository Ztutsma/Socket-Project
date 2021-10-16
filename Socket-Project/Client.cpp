// Socket-Project.cpp : Defines the entry point for the application.
//

#include "Client.h"


int main(int argc, char* argv[])
{
	// Set Server Port & IP Address
	std::string serverIP;
	int serverPort;
	bool servIPValid = false;

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
		std::cin >> serverIP;

		servIPValid = ValidateIPAddress(serverIP);
	}

	while (serverPort < 28500 || serverPort > 28999)
	{
		std::cout << "Please enter a valid port number for the server (28500-28999): ";
		std::cin >> serverPort;
	}

	std::cin.clear();
	std::cin.ignore(10000, '\n');

	// Create Client
	Client* client = new Client(serverIP, serverPort);


	// Begin client user input loop
	std::string userInput = "";
	std::vector<std::string> args;

	while (!(strcasecmp(userInput.c_str(), "quit") == 0) && !(strcasecmp(userInput.c_str(), "exit") == 0))
	{
		std::cout << "IN >> ";
		std::getline(std::cin, userInput);

		// Remove leading whitespace
		userInput = userInput.substr(userInput.find_first_not_of(" \n\t\r\f\v"));

		std::cin.clear();

		// Parse user input into list of arguments
		args = ParseUInput(userInput);

		if (strcasecmp(args[0].c_str(), "register") == 0)
		{
			client->RequestRegister(args);
		}
		if (strcasecmp(args[0].c_str(), "deregister") == 0)
		{
			client->RequestDeregister(args);
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
}

Client::~Client()
{
}

bool Client::RequestRegister(std::vector<std::string> args)
{
	// Format message

	// Message server

	// Wait for response

	// Check if successful

		// Set Self's Username, IP Address, & Ports

		// Start Client

}

bool Client::RequestDeregister(std::vector<std::string> args)
{
	// Format message

	// Message server

	// Wait for response

	// Check if successful

}

bool Client::RequestDHTSetup(std::vector<std::string> args)
{
	// Format message

	// Message server

	// Wait for response

	// Parse response

	// Check if successful

		// Set self as leader

		// Add peers to list of nodes in network

		// Build DHT Network 

		// Build DHT

}

void Client::RequestJoinDHT(std::vector<std::string> args)
{
	// Format message

	// Message server

	// Wait for response

	// Parse Message

	// Check if successful

		// Tell leader to teardown dht

		// Wait for tearddown to complete

		// Tell leader to add ID to DHT network

		// Wait for id to be set

		// Tell leader to rebuild DHT

		// Wait for DHT to be rebuilt

		// Tell Server DHT has been rebuilt
}

void Client::RequestLeaveDHT(std::vector<std::string> args)
{
	// Format message

	// Message server

	// Wait for response

	// Check if successful

		// Begin tearddown

		// Wait for tearddown to return to self

		// Tell right neighbour to reset ID
		
		// Wait for reset ID to return to self

		// Reset left neighbour

		// Reset right neighbour

		// Tell Right neighbour to rebuild DHT

		// Wait for DHT to be rebuilt

		// Tell Server DHT has been rebuilt

}

void Client::RequestRebuildDHT()
{

}

void Client::RequestDHTTearddown()
{

}

bool Client::RequestQueryDHT(std::vector<std::string> args)
{
	// Format message

	// Message server

	// Wait for response

	// Check if successful

}

void Client::QueryDHT(std::vector<std::string> args)
{
	// Format Message

	// Message DHT member

	// Wait for response

	// Print Response
}

void Client::StartClient()
{
	// Build Sockets

	// Start Listening Threads

}

void Client::BuildLeftSocket()
{

}

void Client::BuildRightSocket()
{

}

void Client::BuildQuerySocket()
{

}

void Client::ListenLeftPort()
{
	// Loop infinitely
	while (true)
	{
		// Listen to port

		// Handle message

	}
}

void Client::ListenQueryPort()
{
	// Loop infinitely
	while (true)
	{
		// Listen to port

		// Handle message

	}
}

void Client::HandleMessage(std::string msg)
{
	// Parse message

	// Switch based off first argument


}

void Client::BuildDHTNetwork()
{
	// Tell peers in network to set IDs and neighbours
}

void Client::BuildDHT()
{
	// Loop

		// Parse CSV data entry

		// Get hashed values from long name

		// Store DHT Entry
}

void Client::RebuildDHT()
{
	// Build DHT

	// Reply w/ username that DHT is complete
}

void Client::SetDHTPeerInfo(std::vector<std::string> args)
{
	// Set self ID

	// Set Right Neighbour IP Addr/Port

	// Set Left Neighbour IP Addr/Port
}

void Client::StoreDHTEntry(std::vector<std::string> args)
{
	// Check if entry belongs to self

		// Add to DHT

	// Else
		
		// Format Message	
	
		// Send through DHT network

}

