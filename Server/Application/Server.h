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

	// @brief Recieves packets from the client.
	void RecievePacketsFromStreamer();

	// @brief Processes prediction of movement and hides any disparities between the client and server.
	void Prediction(const float deltaTime, sf::RectangleShape graphics[6], std::vector<GameData>& messages);

	// @brief Function to generate level assets based on the description given.
	void GenerateAssets();

	// @brief Removes the connection and shuffles the array to fit the new amount of connections.
	void RemoveConnection(sf::Uint32 element);

private:

	// @brief Prediction functions to smooth out desparities between client and server.
	inline sf::Vector2f LinearPrediction(const GameData& messageA, const GameData& messageB, const float currentTime);
	inline sf::Vector2f QuadraticPrediction(const GameData& messageA, const GameData& messageB, const GameData& messageC, const float currentTime);

	// @brief A listener socket, used to listen for new connections.
	sf::TcpListener mListener;

	// @brief Select object to query sockets for read or write operations.
	sf::SocketSelector mSelect;

	// @brief Array of game updates.
	std::vector<GameData> mMessages;

	// @brief Array of connections to the server.
	std::vector<Connection*> mConnections;

	std::vector<ChatMSG> mChatLog;

	// @brief A variable to track latency with respect to the last two packets recieved.
	float latency;

	// @brief A variable to track jitter from the last N packets.
	float jitter;

	// @brief Structure containing a description of the game assets.
	AssetData mAssetData;
};

