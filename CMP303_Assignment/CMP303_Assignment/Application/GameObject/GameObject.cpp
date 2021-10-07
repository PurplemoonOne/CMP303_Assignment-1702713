#include "GameObject.h"

UINT32 GameObject::objectCount = 0;

GameObject::GameObject()
{
	objectCount += 1;
	objectId = 1;
	objectId << objectCount;
}
