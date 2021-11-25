#pragma once
#include <SFML\Network.hpp>
#include "../Log/Log.h"
#include <chrono>

enum class ClientPrivelage
{
	None = 0,
	Stream,
	Spectate
};

typedef unsigned short int AssetType;
typedef unsigned short int AssetCount;

typedef std::chrono::system_clock::time_point Time;
typedef std::chrono::system_clock Clock;

// @brief Base packet for all data sent/recieved from the server.
struct BaseData
{
	double time;
};

struct GameData : public BaseData
{
	sf::Uint32 id;
	float x;
	float y;
};

// @brief Special packet structure for the server to send the client confirming a packet was recieved.
struct ServerPingMSG : public BaseData
{
	const char* message = "Packet Confirmed.";
};

struct ConnectionData : public BaseData
{
	sf::Uint8 privelage;	
};

struct ChatMSG : public BaseData
{
	sf::String message;
	sf::Uint32 id;
	sf::Uint32 quit = 0;
};

struct AssetData : public BaseData
{
	AssetType type;
	AssetCount count;
	float sizeX;
	float sizeY;
};

typedef std::initializer_list<AssetData> AssetList;

