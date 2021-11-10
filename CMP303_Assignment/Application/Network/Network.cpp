#include "Network.h"
#include "../Log/Log.h"

inline bool Network::CreateUDPSocket()
{
	// Create sockets and bind port numbers not used by any major application.

	if (socket.bind(port, ipAdress) != sf::Socket::Done)
	{
		//Can't bind port number, so we can request sfml to grab any port
		//number that is not bound to a socket.
		APP_ERROR("Could not bind socket to port %i", port);
		APP_TRACE("Fetching new port number...");
		if (socket.bind(sf::Socket::AnyPort) != sf::Socket::Done)
		{
			//In case something really wrong happens we kill any attempt
			//to connect to a peer and return to the main menu.
			APP_ERROR("Something went wrong, not ports are available...");
		}
	}
}

inline void Network::CloseSocket()
{
	socket.unbind();
}
