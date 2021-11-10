#pragma once

class Entity;
class Entity;
class Event
{
public:
	virtual ~Event() = default;
	virtual void Execute(const float deltaTime, Entity* entity) = 0;
	virtual void Execute(const float deltaTime, Entity* entity, Entity* other) = 0;
	virtual void Execute(const float deltaTime, Entity* entity, float x0, float x1) = 0;
};