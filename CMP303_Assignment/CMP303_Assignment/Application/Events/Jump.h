#pragma once
#include "Event.h"
#include "../Entity/Entity.h"

class Jump : public Event
{
public:

	virtual ~Jump() = default;

	virtual void Execute(const float deltaTime, Entity* entity) override
	{

	}

};