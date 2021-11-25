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
	void SendGamePacket(std::pair<float, float> position);

	bool SendChatMessage(const sf::Text& message);

	// @brief Inform the server about the assets we wish to predict positions etc for.
	void SendAssetsToServer(AssetList assetTypes);

	// @brief Recieve a packet from the host.
	void RecievePacket();

	// @brief Establish a connection with the server.
	void ConnectToServer();

	// @brief Ask the server to end the stream, hence close the socket gracefully.
	void Disconnect();

public:

	/**		Getters	& Setters	**/
	const sf::UdpSocket& GetUDPSocket() const { return mUDPSocket; }

	const sf::TcpSocket& GetTCPSocket() const { return mTCPSocket; }

	// @brief Return the IP Address of the client.
	const sf::IpAddress& GetIPAdress() const { return mIPAdress; }

	// @brief Set the client's privelage.
	void SetClientPrivelage(ClientPrivelage privelage) { mPrivelage = privelage; }

	// @brief Set user name.
	void SetUserName(const sf::String& name) { mUserName = name; }

	// @brief Get the username of the client.
	const sf::String& GetClientUserName() const { return mUserName; }

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

	// @brief User name for the client.
	sf::String mUserName;

	// @brief A variable to hold the most up to date latency value.
	float mLatency;
	// @brief A variable to hold the average jitter based on the last N messages.
	float mJitter;
};