// Socket-Project.cpp : Defines the entry point for the application.
//

#include "Client.h"


int main()
{
	return 0;
}

Client::Client()
{
	// Build Server Socket

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

bool Client::RequestDeregister()
{
	// Format message

	// Message server

	// Wait for response

	// Check if successful

}

bool Client::RequestDHTSetup()
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

void Client::RequestJoinDHT()
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

void Client::RequestLeaveDHT()
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

bool Client::RequestQueryDHT()
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

