#pragma once
#include "NetworkUtilities.h"

class Connection
{
public:
	Connection(sf::TcpSocket& socket);
	~Connection() = default;

	sf::TcpSocket& GetSocket() { return *mTCPSocket.get(); }

	const sf::IpAddress& GetConnectionIP() const { return mIPAdress; }

	const uint16_t GetConnectionPort() const { return mPort; }

	const uint16_t GetNetworkID() const { return mNetworkID; }

	// @brief For the host to update the client this connection belongs to.
	void Send(uint16_t hostPort, uint16_t networkID);

private:

	// @brief A pointer to the client's socket. (sf::Socket non-copyable)
	std::unique_ptr<sf::TcpSocket> mTCPSocket;

	// @brief The client's machine IP
	sf::IpAddress mIPAdress;

	// @brief This connection's port number.
	uint16_t mPort = 0;

	uint16_t mNetworkID;

};