#pragma once
#include <SFML\Network.hpp>
#include "../Log/Log.h"

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

enum class Privelage
{
	None = 0,
	Host = 0x1000,
	Client = 0x0001
};
