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
	void SendGamePacket(std::pair<float, float> position, sf::Uint32 id);

	// @brief Recieve a TCP packet from the server.
	ConnectionData& RecieveHostAssets();

	// @brief Recieve a packet from the host.
	void RecievePacket();

	// @brief Fetch any new avaliable ports to send data to.
	void GatherNewPorts();

	// @brief Establish a connection with the server.
	void ConnectToServer(AssetType assetType = 0, AssetCount assetCount = 0, sf::Vector2f assetSize = sf::Vector2f());

	// @brief Ask the server to end the stream, hence close the socket gracefully.
	bool Disconnect();

	std::vector<GameData>& GetGameData() { return mGameData; }

public:
	/**		Getters	& Setters	**/
	const sf::UdpSocket& GetUDPSocket() const { return mUDPSocket; }

	const sf::TcpSocket& GetTCPSocket() const { return mTCPSocket; }

	// @brief Return the IP Address of the client.
	const sf::IpAddress& GetIPAdress() const { return mIPAdress; }

	// @brief Set the client's privelage.
	void SetClientPrivelage(ClientPrivelage privelage) { mPrivelage = privelage; }

private:
	// @brief The client's machine IP
	sf::IpAddress mIPAdress, mServerIpAddress;

	// @brief This client's socket for send and recieving data.
	sf::UdpSocket mUDPSocket;

	// @brief A socket to connect to the host.
	sf::TcpSocket mTCPSocket;

	// @brief This connection's port number.
	sf::Uint16 mUDPPort = 0000, mServerUDPPort = 0000;

	// @brief Clients privelage
	ClientPrivelage mPrivelage;

	// @brief An array of all the ports and Ip address to send game updates to.
	std::vector<std::pair<sf::Uint16, sf::IpAddress>> mPeers;

	// @brief An array holding game updates.
	std::vector<GameData> mGameData;

	// @brief A variable to hold the most up to date latency value.
	float mLatency;
	// @brief A variable to hold the average jitter based on the last N messages.
	float mJitter;
};