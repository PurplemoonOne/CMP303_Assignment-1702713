#pragma once
#include <SFML\Network.hpp>
#include "../Log/Log.h"

enum class ClientPrivelage
{
	None = 0,
	Stream,
	Spectate
};

enum class AssetType
{
	None = 0,
	Rect,
	Circle,
	Texture,
	Sprite,
	SFX
};

typedef std::initializer_list<std::pair<AssetType, uint32_t>> AssetList;

struct Data
{
	//SFML implements types with respect to the platform .
	//Still requires caution.
	sf::Uint32 time;

	//According to the documentation, it is okay to use 'float and double' as
	//these are mostly sized as 32-bits and 64-bits respectively.
	float x;
	float y;
};

class GamePacket : public sf::Packet
{

public:
	GamePacket() = default;
	GamePacket(const GamePacket& other) = default;
	~GamePacket() = default;

	sf::Packet& operator<<(sf::Packet& packet, const Data& data)
	{
		return packet << data.time << data.x << data.y;
	}

};

class AssetPacket : public sf::Packet
{
public:
	AssetPacket() = default;
	AssetPacket(const AssetPacket& other) = default;
	~AssetPacket() = default;

	AssetList Decompress(sf::Packet packet)
	{
		sf::Uint32 count;
		AssetType type;
		size_t count = (packet.getDataSize() / (sizeof(sf::Uint32) + sizeof(AssetType)));

		for (int i = 0; i < count; ++i)
		{

		}

		return ;
	}

};
