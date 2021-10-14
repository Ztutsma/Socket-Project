#include "Server.h"

int main() 
{
	return 0;
}

Server::Server(int port)
{
	// Build Server Socket

}

Server::~Server()
{

}

void Server::StartServer()
{
	// Run threads

}

void Server::ListenToPort()
{
	// Loop infinitely
	while (true)
	{
		// Listen to port

		// Handle message

		// Reply to client
	}
}

std::string Server::HandleMessage(std::string msg)
{
	// Check if server is accepting messages
	// Proceed if dht is not being built nor rebuilt

	// Parse message into list of arguments

	// Switch based on first argument

	// Return response

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