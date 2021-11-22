#pragma once
#include <SFML\Network.hpp>
#include "../Log/Log.h"

enum class ClientPrivelage
{
	None = 0,
	Stream,
	Spectate
};

struct GameData
{
	//SFML implements types with respect to the platform .
	//Still requires caution.
	float appElapsedTime;
	sf::Uint32 id;

	//According to the documentation, it is okay to use 'float and double' as
	//these are mostly sized as 32-bits and 64-bits respectively.
	float x;
	float y;
};

typedef unsigned short int AssetType;
typedef unsigned short int AssetCount;

// @brief Special packet structure for the server to send the client confirming a packet was recieved.
struct ServerPingMSG
{
	const char* message = "Packet Good";
	float timeStamp;
};

struct ChatMSG
{
	sf::String message;
	float timeStamp;
	sf::Uint32 id;
	sf::Uint32 quit = 0;
};

struct AssetData
{
	AssetType type;
	AssetCount count;
	float sizeX;
	float sizeY;
};

typedef std::initializer_list<AssetData> AssetList;

class GamePacket : public sf::Packet
{
public:
	GamePacket() = default;
	GamePacket(const GamePacket& other) = default;
	~GamePacket() = default;
};

class AssetPacket : public sf::Packet
{
public:
	AssetPacket() = default;
	AssetPacket(const AssetPacket& other) = default;
	~AssetPacket() = default;
};

