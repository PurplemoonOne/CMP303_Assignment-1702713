#pragma once
#include "Event.h"
#include "../GameObject/Entity.h"

class Move : public Event
{
public:

	virtual ~Move() = default;

	virtual void Execute(const float deltaTime, Entity* entity) override
	{
		entity->GetTransform(entity->GetTag()).position.x += 100.0f * deltaTime;
	}

	virtual void Execute(const float deltaTime, Entity* entity, Entity* other) override
	{

	}

	virtual void Execute(const float deltaTime, Entity* entity, float x0, float x1) override
	{
		sf::Vector2f direction = sf::Vector2f(x0, x1) - entity->GetTransform(entity->GetTag()).position;
		entity->GetTransform(entity->GetTag()).position += direction * 100.0f * deltaTime;
	}

};