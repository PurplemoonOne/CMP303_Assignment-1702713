#pragma once

class Entity;
class Event
{
public:
	virtual ~Event() = default;
	virtual void Execute(const float deltaTime, Entity* entity) = 0;
};