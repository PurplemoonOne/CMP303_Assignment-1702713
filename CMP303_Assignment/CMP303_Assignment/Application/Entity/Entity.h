//#pragma once
//#include "../ECS/EntityComponentSystem.h"
//#include "../Log/Log.h"
//#include <memory>
//
//class Scene;
//class Entity
//{
//public:
//	Entity() = default;
//	Entity(std::shared_ptr<EntityComponentSystem>* lecs, Scene* scene);
//	Entity(const Entity& entity);
//	Entity(const Entity&& entity);
//	virtual ~Entity() = default;
//public:
//	
//	
//	template<typename T, typename ... Args> T& AddComponent(Args&&... args)
//	{
//		return scene->ecs->Emlpace<T>(id, std::forward<Args>...);
//	}
//
//	template<typename T> T& GetComponent() const
//	{
//		ASSERT(!HasComponent<T>(), "Entity does not have this component.");
//		return scene->ecs.Get<T>(id);
//	}
//
//	template<typename T> bool HasComponent() const
//	{
//		bool exists = scene->system.Get<T>(id);
//		return exists;
//	}
//
//private:
//	unsigned int GetID() const 
//	{
//		return id;
//	}	
//private:
//	Scene* scene;
//	unsigned int id;
//};
//
