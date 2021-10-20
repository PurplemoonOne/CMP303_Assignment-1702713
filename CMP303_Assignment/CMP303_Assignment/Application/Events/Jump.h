#pragma once
#include "Event.h"
#include "../GameObject/Entity.h"

class Jump : public Event
{
public:

	virtual ~Jump() = default;

	virtual void Execute(const float deltaTime, Entity* entity) override
	{
		entity->GetTransform(entity->GetTag()).position.x += 5.0f * deltaTime;
	}


};