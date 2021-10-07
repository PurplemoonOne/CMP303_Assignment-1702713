#pragma once
#include <bits.h>


class GameObject
{
public:
	GameObject();
	GameObject(const GameObject& gameObject) = default;
	~GameObject() = default;
public:

	inline int ID() const { return objectId; }

private:
	static UINT32 objectCount;
	UINT32 objectId;
};

