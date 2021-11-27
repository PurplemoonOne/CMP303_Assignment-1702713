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
	inline void InitArray(sf::Uint32 arSize)
	{
		x = new float[arSize];
		y = new float[arSize];
		objectIDs = new sf::Uint32[arSize];
		arraySize = arSize;
	}

	inline void DeleteData()
	{
		if (x) delete x;
		if (y) delete y;
		if (objectIDs) delete objectIDs;
	}

	sf::Uint16 peerUdpRecvPort;
	sf::Uint32 arraySize;
	sf::Uint32* objectIDs; 
	float* x;
	float* y;
	
};

typedef unsigned short int AssetType;
typedef unsigned short int AssetCount;

struct ConnectionData : public BaseData
{
	sf::Uint8 privelage;
	sf::Uint16 peerUdpRecvPort;
	sf::Uint32 ipAddress;
	AssetType type;
	AssetCount count = 0;
	float sizeX;
	float sizeY;
};

struct ClientPortAndIP
{
	sf::Uint16 udpPort;
};

struct DisconnectPCKT : public BaseData
{
	sf::String message;
	sf::Uint32 id;
	sf::Uint32 quit = 0;
};


