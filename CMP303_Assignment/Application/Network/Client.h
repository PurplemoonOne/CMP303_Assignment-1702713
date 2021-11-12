#pragma once
#include "NetworkUtilities.h"

class Client
{
public:
	Client();
	Client(const Client&) = default;
	~Client();

	// @brief Send a packet to a specified connection.
	void SendPacket(std::pair<float, float> position, const float timeStamp, const int32_t id, const uint16_t port);

	// @brief Recieve a packet from the host.
	void RecievePacket();

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
	inline void SetIPAddress(sf::IpAddress ipAddress) { mIPAdress = ipAddress; }
	inline void SetHostIPAddress(sf::IpAddress hipAddress) { mHostIpAddress = hipAddress; }
	inline void setHostPort(uint16_t hport) { mHostPort = hport; }

	const sf::IpAddress& GetHostIPAddress() const { return mHostIpAddress; }
	const uint16_t GetHostPort() const { return mHostPort; }

private:
	// @brief The client's machine IP
	sf::IpAddress mIPAdress, mHostIpAddress = " ";

	// @brief This connection's socket for send and recieving data.
	sf::UdpSocket mSocket;

	// @brief This connection's port number.
	uint16_t mPort = 0, mHostPort = 1;

	// @brief tracks whether this connection is the host or client.
	Privelage mPrivelage;

	// @brief A vector containing most recent N messages.
	std::vector<Packet> mMessages;

};