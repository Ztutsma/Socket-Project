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
		return "Failure";
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
	// Check for unique username

	// Check for username length

	// Check for no IP Address and Port conflicts

	// Add to list of registered peers
}

std::string Server::DeregisterPeer(std::vector <std::string> args)
{
	// Check if peer is in DHT

	// Remove from list of registered peers

}

std::string Server::StartDHTSetup(std::vector <std::string> args)
{
	// Check if peer is registered

	// Check if DHT already exists

	// Check if there are more than 2 nodes being requested

	// Check if there are at least as many peers as nodes being requested

	// Set peer to leader

	// Select nodes to be in DHT

	// Set DHT status as being built

	// Format message

	// Return message containing tuples of nodes in DHT

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
	// DHT complete

		// Check if peer is leader

		// Mark DHT as running

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