#pragma once
#include "SFML/Network.hpp"

struct Packet
{
	int32_t id;
	float x;
	float y;
	float time;
};

class Network
{
public:
	Network() = default;
	~Network() = default;

public:

	inline bool CreateUDPSocket();
	inline void CloseSocket();

	const sf::UdpSocket& GetSocket() const { return socket; }
	sf::UdpSocket& GetSocket() { return socket; }

private:
	sf::UdpSocket socket;
	int32_t port;
	std::string ipAdress;

	// @brief A vector containing most recent N messages.
	std::vector<Packet> messages;

	// @brief A variable to hold the most up to date latency value.
	float latency;
	// @brief A variable to hold the average jitter based on the last N messages.
	float jitter;
};