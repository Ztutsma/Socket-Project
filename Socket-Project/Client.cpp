// Socket-Project.cpp : Defines the entry point for the application.
//

#include "Client.h"

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

	return true;
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