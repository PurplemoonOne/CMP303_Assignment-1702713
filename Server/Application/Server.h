#pragma once
#include "SFML\Graphics.hpp"
#include "../Network/Connection.h"


struct Game
{
	Game(Connection* host = nullptr, Connection* client = nullptr)
		:
		mHost(host),
		mClient(client)
	{
		if (mHost && mClient)
		{
			//This game contains a pair.
			mHasClient = true;
			mHasHost = true;
		}
	}
	~Game() = default;

	Connection* mHost = nullptr;
	Connection* mClient = nullptr;
	bool mHasHost = false;
	bool mHasClient = false;

	void StoreAssetData(ConnectionData& data)
	{
		if (data.privelage == 0)
		{
			mHostAssets.count = data.count;
			mHostAssets.type = data.type;
			mHostAssets.sizeX = data.sizeX;
			mHostAssets.sizeY = data.sizeY;
			mHostAssets.ipAddress = data.ipAddress;
			mHostAssets.peerUdpRecvPort = data.peerUdpRecvPort;
			mHasHostAssets = true;
		}
		else
		{
			mClientAssets.count = data.count;
			mClientAssets.type = data.type;
			mClientAssets.sizeX = data.sizeX;
			mClientAssets.sizeY = data.sizeY;
			mClientAssets.ipAddress = data.ipAddress;
			mClientAssets.peerUdpRecvPort = data.peerUdpRecvPort;
			mHasClientAssets = true;
		}
	}

	// @brief A bool to check whether the asset description have been stored on the server.
	bool mHasHostAssets = false;
	bool mHasClientAssets = false;

	// @brief Copy of the asset description.
	ConnectionData mHostAssets{};
	ConnectionData mClientAssets{};
};

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

	// @brief Gather data about the client in question. i.e Privelages...
	void InitConnection(sf::TcpSocket* socket);

private:

	// @brief A listener socket, used to listen for new connections.
	sf::TcpListener mListener;

	// @brief Select object to query sockets for read or write operations.
	sf::SocketSelector mSelect;

	// @brief Array of connections to the server.
	std::array<Game, 10> mConnections;


	sf::Uint32 mHostCount = 0;
	sf::Uint32 mClientCount = 0;
	sf::Uint32 mTotalConnections = 0;

};

