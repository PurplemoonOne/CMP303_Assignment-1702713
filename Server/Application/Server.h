#pragma once
#include "SFML\Graphics.hpp"
#include "../Network/Connection.h"

#define lerp(a,b,t) a * (1 - t) + b * t

class Server
{
public:
	Server();
	~Server();

	void Run();

private:

	// @brief Polls window events and processes them accordingly.
	void ProcessEvents(sf::RenderWindow& window);

	// @brief Creates the listener object and waits for a connection.
	void ListenForConections();

	// @brief Checks for any new connections and accepts them.
	void QueryConnections();

	// @brief Function to generate level assets based on the description given.
	void StoreClientAssetData(ConnectionData& data);

	// @brief Removes the connection and shuffles the array to fit the new amount of connections.
	void RemoveConnection(sf::Uint32 element);

	// @brief Gather data about the client in question. i.e Privelages...
	void InitConnection(sf::TcpSocket* socket);

private:

	// @brief A listener socket, used to listen for new connections.
	sf::TcpListener mListener;

	// @brief Select object to query sockets for read or write operations.
	sf::SocketSelector mSelect;

	// @brief Array of connections to the server.
	std::vector<Connection*> mConnections;


	// @brief A variable to track latency with respect to the last two packets recieved.
	float mLatency;

	// @brief A variable to track jitter from the last N packets.
	float mJitter;

	// @brief A bool to check whether the assets have been initialised on the server.
	bool mHasAssets = false;

	std::vector<sf::RectangleShape> mGraphics;
	sf::Uint32 mHostCount = 0;
	sf::Uint32 mClientCount = 0;
	sf::Uint32 mTotalConnections = 0;
	ConnectionData mAssets{};
};

