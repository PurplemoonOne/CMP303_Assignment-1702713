#pragma once
#include "Event.h"
#include "../GameObject/Entity.h"

class Move : public Event
{
public:

	virtual ~Move() = default;

	virtual void Execute(const float deltaTime, uint16_t networkID, Entity* entity) override
	{
		entity->GetTransform(networkID).position.x += 100.0f * deltaTime;
	}


	virtual void Execute(const float deltaTime, Entity* entity, float x0, float x1) override
	{
		sf::Vector2f direction = sf::Vector2f(x0, x1) - entity->GetTransform().position;
		entity->GetTransform().position += direction * 0.2f * deltaTime;
	}

};