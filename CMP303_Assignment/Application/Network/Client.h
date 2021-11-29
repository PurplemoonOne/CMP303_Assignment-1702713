#pragma once
#include "NetworkUtilities.h"
#include "SFML/Graphics/Text.hpp"

class Client
{
public:
	Client();
	Client(const Client&) = default;
	~Client();

	// @brief Send a packet to a specified connection.
	void SendGamePacket(std::vector<sf::Vector2f> positions,const float appElapsedTime);

	void SendConnectionInformation(AssetType assetType = 0, AssetCount assetCount = 0, sf::Vector2f assetSize = sf::Vector2f());

	// @brief Recieve game assets from the server.
	ConnectionData& RecieveAssetsDescFromServer();

	// @brief Recieve a packet from other peers.
	void RecievePacket();

	// @brief Establish a connection with the server.
	void ConnectToServer();

	// @brief Ask the server to end the stream, hence close the socket gracefully.
	bool Disconnect();

	std::vector<GameData>& GetGameData() { return mGameData; }

	const sf::Uint32 GetPeerCount() const { return mPeers.size(); }

public:
	/**		Getters	& Setters	**/
	const sf::UdpSocket& GetUDPSendSocket() const { return mUDPSendSocket; }
	const sf::UdpSocket& GetUDPRecvSocket() const { return mUDPRecvSocket; }
	const sf::TcpSocket& GetTCPSocket() const { return mTCPSocket; }
	const sf::IpAddress& GetIPAdress() const { return mIPAdress; }
	void SetClientPrivelage(ClientPrivelage privelage) { mPrivelage = privelage; }

private:

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

	// @brief An array of all the ports and Ip address to send game updates to.
	std::vector<sf::Uint16> mPeers;

	// @brief An array holding game updates.
	std::vector<GameData> mGameData;

	// @brief A variable to hold the most up to date latency value.
	float mLatency;
	// @brief A variable to hold the average jitter based on the last N messages.
	float mJitter;
};