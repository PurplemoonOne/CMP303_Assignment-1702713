#pragma once
#include <SFML\Network.hpp>

// @note Forcing the compiler to pack 
// the struct in bytes of 4.
#pragma pack(show) 
#pragma pack(push, 4)
struct Packet
{
	// @brief instructions on unpacking the raw bytes.
	const char ins[4] = { 'I', 'T', 'X', 'Y' };
	int32_t id;
	float time;
	float x;
	float y;
};
#pragma pack(pop)


// @brief The connection class handles creating connections and 
// tracking the connections peformance i.e calculating the latency and jitter.
class Connection
{
public:
	Connection() = default;
	Connection(const Connection&) = default;
	Connection(const sf::IpAddress& ipAdress);
	~Connection() = default;

public:

	bool CreateUDPSocket(uint16_t port);
	void CloseSocket();

	// @brief Send a packet to a specified connection.
	void SendPacket(const float positionX, const float positionY, const float timeStamp, const int32_t id, const uint16_t port);
	void SendPacket(const Packet& packet, uint16_t port);

	// @brief Check for any valid data coming through to this socket.
	void ListenForPacket(uint16_t port);

	const sf::UdpSocket& GetSocket() const { return mSocket; }
	sf::UdpSocket& GetSocket() { return mSocket; }

private:
	sf::UdpSocket mSocket;
	sf::IpAddress mIPAdress;
	uint16_t mPort = 0;

	// @brief A vector containing most recent N messages.
	std::vector<Packet> mMessages;
	// @brief A variable to hold the most up to date latency value.
	float mLatency;
	// @brief A variable to hold the average jitter based on the last N messages.
	float mJitter;
};