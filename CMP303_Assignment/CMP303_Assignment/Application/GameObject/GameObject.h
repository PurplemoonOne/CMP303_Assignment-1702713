#pragma once
#include <SFML/System/Vector2.hpp>

class GameObject
{
public:
	GameObject() = default;
	GameObject(const GameObject& gameObject) = default;
	GameObject(float health, float dps);
	~GameObject();
public:



private:
	
	int objectId;
};

