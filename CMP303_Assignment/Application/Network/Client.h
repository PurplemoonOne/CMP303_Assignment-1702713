#pragma once
#include "NetworkUtilities.h"
#include "SFML/Graphics/Text.hpp"

struct Peer
{
	sf::Uint32 ipAddress;
	sf::Uint16 port;
};

class Client
{
public:
	Client(sf::Vector2f windowMaxBoundaries);
	Client(const Client&) = default;
	~Client();

	// @brief Send a packet to a specified connection.
	void SendGamePacket(std::vector<sf::Vector2f>& positions, std::vector<float>& rotations, std::vector<std::pair<float, float>>& scales);

	void SendConnectionInformation(AssetType assetType = 0, AssetCount assetCount = 0, sf::Vector2f assetSize = sf::Vector2f());

	// @brief Recieve game assets from the server.
	ConnectionData& RecieveAssetsDescFromClient();

	// @brief Recieve a packet from other peers.
	void RecieveGameUpdate();

	// @brief Establish a connection with the server.
	void ConnectToServer();

	void RecieveConnectionStatusFromServer();

	// @brief Ask the server to end the stream, hence close the socket gracefully.
	bool Disconnect();

	std::vector<GameData>& GetGameData() { return mGameData; }
	const sf::Vector2f GetPredictedPosition() const { return mPredictedPositions.back(); }
	
public:
	/**		Getters	& Setters	**/
	const sf::UdpSocket& GetUDPSendSocket() const { return mUDPSendSocket; }
	const sf::UdpSocket& GetUDPRecvSocket() const { return mUDPRecvSocket; }
	const sf::TcpSocket& GetTCPSocket() const { return mTCPSocket; }
	const sf::IpAddress& GetIPAdress() const { return mIPAdress; }
	void SetClientPrivelage(ClientPrivelage privelage) { mPrivelage = privelage; }

	// @brief Grabs the most up to date latency peformance.
	const float GetRecentLatency() const { return mLatency; }

	// @brief Check if the client has left the game.
	bool GetHasClientQuit() const { return mPeerQuit; }
	
private:

	// @brief Verifies the packet recieved is valid and stores into the 
	// game update array.
	void StoreGameUpdate(const GameData& data);

	// @brief Initialises the UDP sockets and binds them to unused ports.
	void BindUDPSockets();

	// @brief The client's machine IP
	sf::IpAddress mIPAdress, mServerIpAddress;

	// @brief This client's socket for send and recieving data.
	sf::UdpSocket mUDPSendSocket;
	sf::UdpSocket mUDPRecvSocket;

	// @brief A socket to connect to the host.
	sf::TcpSocket mTCPSocket;

	// @brief Select object to query whether a socket is ready to read.
	sf::SocketSelector mSelect;

	// @brief This connection's port number.
	sf::Uint16 mUDPSendPort = 0000;
	sf::Uint16 mUDPRecvPort = 0000;

	// @brief Clients privelage
	ClientPrivelage mPrivelage;

	// @brief Object representing the other connection.
	Peer mPeer;

	// @brief An array holding game updates.
	std::vector<GameData> mGameData;

	// @brief Predicted Position from client.
	std::vector<sf::Vector2f> mPredictedPositions;

	// @note Some data to keep track of the valid number of data.

	sf::Uint32 mAssetCount;

	// @brief Scale along axis are the same so need only track one edge.
	int mAssetScale = 1;

	// @brief Max level boundaries.
	sf::Vector2f mWindowMaxBoundary;

	// @brief Time difference between sending a packet and
	// connection recieving the packet.
	float mLatency;

	bool mPeerQuit = false;

};