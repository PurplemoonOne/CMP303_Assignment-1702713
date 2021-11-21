#pragma once
#include "NetworkUtilities.h"


class Client
{
public:
	Client();
	Client(const Client&) = default;
	~Client();

	// @brief Send a packet to a specified connection.
	void SendPacket(std::pair<float, float> position, const float timeStamp);

	// @brief Recieve a packet from the host.
	void RecievePacket();

	// @brief Establish a connection with the server.
	void ConnectToServer();

	// @brief Ask the server to end the stream, hence close the socket gracefully.
	void Disconnect();

	// @brief Inform the server about the assets we wish to predict positions etc for.
	void SendAssetsToServer(AssetList assetTypes);

public:

	/**		Getters	& Setters	**/

	const sf::UdpSocket& GetSocket() const { return mUDPSocket; }
	sf::UdpSocket& GetSocket() { return mUDPSocket; }

	// @brief Return the client's porrt number.
	const uint16_t GetLocalPort() const { return mUDPSocket.getLocalPort(); }
	uint16_t GetLocalPort() { return mUDPSocket.getLocalPort(); }

	// @brief Return the IP Address of the client.
	const sf::IpAddress& GetIPAdress() const { return mIPAdress; }
	sf::IpAddress& GetIPAdress() { return mIPAdress; }

	// @brief Set the client's privelage.
	void SetClientPrivelage(ClientPrivelage privelage);

private:
	// @brief The client's machine IP
	sf::IpAddress mIPAdress, mHostIpAddress = " ";

	// @brief This client's socket for send and recieving data.
	sf::UdpSocket mUDPSocket;

	// @brief A socket to connect to the host.
	sf::TcpSocket mTCPSocket;

	// @brief Select object to check if a connection is ready to read.
	sf::SocketSelector mSelect;

	// @brief For Tcp connection, tracks how much data we have read/written and still to read/write
	size_t mDataBuffer, mDataCount = 0;

	// @brief This connection's port number.
	uint16_t mPort = 0;

	// @brief Clients privelage
	ClientPrivelage mPrivelage;

	// @brief A variable to hold the most up to date latency value.
	float mLatency;
	// @brief A variable to hold the average jitter based on the last N messages.
	float mJitter;
};