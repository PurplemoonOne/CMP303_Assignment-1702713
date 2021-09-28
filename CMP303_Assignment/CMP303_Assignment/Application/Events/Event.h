#pragma once

class Entity;
class GameObject;
class Event
{
public:
	virtual ~Event() = default;
	virtual void Execute(const float deltaTime, Entity* entity) = 0;
	virtual void Execute(const float deltaTime, GameObject* entity) = 0;
};