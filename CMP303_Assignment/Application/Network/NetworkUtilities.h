#pragma once
#include <SFML\Network.hpp>
#include "../Log/Log.h"
#include <chrono>

enum class ClientPrivelage
{
	None = 0,
	Host,
	Client
};

typedef std::chrono::system_clock::time_point Time;
typedef std::chrono::system_clock Clock;

// @brief Base packet for all data sent/recieved from the server.
struct BaseData
{
	double time;
};

// @brief Packet containing positional data of the objects.
struct GameData : public BaseData
{
	inline void InitArray(sf::Uint32 arraySize)
	{
		x = new float[arraySize];
		y = new float[arraySize];
		objectIDs = new sf::Int32[arraySize];
	}

	sf::Int32* objectIDs; 
	sf::Uint16 state;
	float* x;
	float* y;
	
};

// @brief Special packet structure for the server to send the client confirming a packet was recieved.
struct ServerPingMSG : public BaseData
{
	const char* message = "Packet Confirmed.";
};

typedef unsigned short int AssetType;
typedef unsigned short int AssetCount;

struct ConnectionData : public BaseData
{
	sf::Uint8 privelage;
	sf::Uint16 udpPort;
	sf::String ipAddress;
	AssetType type;
	AssetCount count = 0;
	float sizeX;
	float sizeY;
};

struct ClientPortAndIP
{
	sf::Uint16 udpPort;
	sf::String ip;
};

struct DisconnectPCKT : public BaseData
{
	sf::String message;
	sf::Uint32 id;
	sf::Uint32 quit = 0;
};


