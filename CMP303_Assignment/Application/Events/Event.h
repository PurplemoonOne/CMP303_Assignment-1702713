#pragma once
#include <cstdint>

class Entity;
class Entity;
class Event
{
public:
	virtual ~Event() = default;
	virtual void Execute(const float deltaTime, uint16_t networkID, Entity* entity) = 0;
	virtual void Execute(const float deltaTime, Entity* entity, float x0, float x1) = 0;
};