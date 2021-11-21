#pragma once
#include "SFML\Graphics.hpp"

#include "../Network/Connection.h"

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
	void AcceptNewConnections();

	// @brief Recieves packets from the client.
	void RecieveIncomingPackets();

	// @brief Processes prediction of movement and hides any disparities between the client and server.
	void Prediction();

	// @brief closes connections gracefully.
	void CloseConnections();

	// @brief Recieve the list of assets from the client.
	void RecieveAssetList();

private:

	std::vector<Connection*> mConnections;

	// @brief A listener socket, used to listen for new connections.
	sf::TcpListener mListener;

	// @brief Select object to query sockets for read or write operations.
	sf::SocketSelector mSelect;

	// @brief Container holding all the drawable objects.
	std::vector<sf::RectangleShape> mDrawables;

	// @brief A variable to track latency.
	float latency;

	// @brief A variable to track jitter.
	float jitter;

	// @brief An array of rectange shapes representing the graphics from the streamer.
	std::vector<sf::RectangleShape> mStreamersGraphics;

};

