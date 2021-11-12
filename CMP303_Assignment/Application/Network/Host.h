#pragma once
#include "Client.h"

class Host
{
public:
	Host();
	Host(const Host&) = default;
	~Host() = default;

public:

	/**		Getters	& Setters	**/

	const sf::UdpSocket& GetSocket() const { return mSocket; }
	sf::UdpSocket& GetSocket() { return mSocket; }

	const uint16_t GetLocalPort() const { return mSocket.getLocalPort(); }
	uint16_t GetLocalPort() { return mSocket.getLocalPort(); }

	const Privelage GetConnectionPrivelages() const { return mPrivelage; }
	Privelage GetConnectionPrivelages() { return mPrivelage; }

	const sf::IpAddress& GetIPAdress() const { return mIPAdress; }
	sf::IpAddress& GetIPAdress() { return mIPAdress; }

	inline void SetPrivelage(Privelage privelage) { mPrivelage = privelage; }
	inline void SetPort(uint16_t port) { mPort = port; }

	// @brief The host can 'listen' for connections and add them to the select object.
	void SearchForConnections();

private:
	// @brief The client's machine IP
	sf::IpAddress mIPAdress;

	// @brief This connection's socket for send and recieving data.
	sf::UdpSocket mSocket;

	// @brief This connection's port number.
	uint16_t mPort = 0;

	// @brief tracks whether this connection is the host or client.
	Privelage mPrivelage;

	sf::SocketSelector mSelect; 

	// @brief A vector containing most recent N messages.
	std::vector<Packet> mMessages;

	// @brief A dynamic array holding all current clients connected to this host. 
	std::vector<Client*> mConnections;

	// @brief A variable to hold the most up to date latency value.
	float mLatency;
	// @brief A variable to hold the average jitter based on the last N messages.
	float mJitter;
};