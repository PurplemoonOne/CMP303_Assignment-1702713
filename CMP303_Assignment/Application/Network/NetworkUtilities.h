#pragma once
#include <SFML\Network.hpp>
#include "../Log/Log.h"

enum class ClientPrivelage
{
	Host = 0,
	Client
};

// @brief Base packet for all data sent/recieved from the server.
struct BaseData
{
	double systemTime;
	double latency;
};

// @brief Packet containing positional data of the objects.
struct GameData : public BaseData
{
	inline void InitArray(sf::Uint32 arSize)
	{
		x = new float[arSize];
		y = new float[arSize];
		rotations = new float[arSize];
		scaleX = new float[arSize];
		scaleY = new float[arSize];
		objectIDs = new sf::Uint32[arSize];
		arraySize = arSize;
	}

	inline void DeleteData()
	{
		if (x)		   delete x;
		if (y)		   delete y;
		if (rotations) delete rotations;
		if (scaleX)	   delete scaleX;
		if (scaleY)    delete scaleY;
		if (objectIDs) delete objectIDs;
	}

	sf::Uint8 quit = 0;
	sf::Uint16 peerUdpRecvPort;
	sf::Uint32 peerIpAddress;
	sf::Uint32 arraySize;
	sf::Uint32* objectIDs; 
	float* x;
	float* y;
	float* rotations;
	float* scaleX;
	float* scaleY;
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

struct LatencyPacket
{
	double timeA;
	double timeB;
};

class GameplayPacket : public sf::Packet
{
public:

	// @brief Overrides sf::Packets call back methods.
	// @note Called before sf::Socket.send(...)
	virtual const void* onSend(size_t& size) override
	{
		const void* srcData = getData();
		size_t srcSize = getDataSize();
		return Encrypt(srcData, srcSize, size);
	}

	// @brief Overrides sf::Packets call back methods.
	// @note Called after sf::Socket.receive(...)
	virtual void onReceive(const void* data, size_t size) override
	{
		size_t dstSize;
		const void* dstData = Decrypt(data, size, dstSize);
		append(dstData, dstSize);
	}
private:

	// @brief Simple encryption function. Used to keep sensitive data safe.
	const void* Encrypt(const void* data, size_t srcSize, size_t& size)
	{
		return data;
	}

	// @brief Simple decrypiton function.
	const void* Decrypt(const void* data, size_t size, size_t& dstSize)
	{
		return data;
	}
};


