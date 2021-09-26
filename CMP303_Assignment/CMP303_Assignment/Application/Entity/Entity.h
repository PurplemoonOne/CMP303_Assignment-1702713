#pragma once

class Entity
{
public:
	Entity() = default;
	Entity(float health);
	Entity(const Entity& entity);
	Entity(const Entity&& entity);
	virtual ~Entity() = default;
public:
	
	float GetJumpForce() const { return jumpForce; }

private:
	float healthPoints;
	float jumpForce;
};

