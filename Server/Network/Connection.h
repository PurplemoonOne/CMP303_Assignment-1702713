#pragma once
#include "../CMP303_Assignment/Application/Network/NetworkUtilities.h"

class Connection
{
public:
	Connection(sf::TcpSocket* socket);
	~Connection() = default;

	sf::TcpSocket* GetTCPSocket() { return mTCPSocket; }
	sf::UdpSocket* GetUDPSocket() { return &mUDPSocket; }

	const sf::IpAddress& GetConnectionIP() const { return mIPAdress; }

	const uint16_t GetConnectionPort() const { return mPort; }

	const uint16_t GetNetworkID() const { return mNetworkID; }

	const ClientPrivelage GetConnectionPrivelage() const { return mConnPrivelage; }

	// @brief For the host to update the client this connection belongs to.
	void Send();

	// @brief Recieve the asset list from the connection.
	AssetList RecieveAssets();

	// @brief Recieve a packet from the connection.
	void Recieve();

private:

	// @brief A pointer to the client's socket. (sf::Socket non-copyable)
	sf::TcpSocket* mTCPSocket;

	sf::UdpSocket mUDPSocket;

	// @brief The client's machine IP
	sf::IpAddress mIPAdress;

	// @brief Clients privelage i.e (The sorts of data they're able to send is tied to their privelage)
	ClientPrivelage mConnPrivelage;

	// @brief This connection's port number.
	uint16_t mPort = 0;

	uint16_t mNetworkID;

};