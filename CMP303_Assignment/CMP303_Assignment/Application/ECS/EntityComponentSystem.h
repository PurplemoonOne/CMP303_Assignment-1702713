//#pragma once
//#include "../Components/Components.h"
//#include "../Log/Log.h"
//#include <vector>
//
//
//class EntityComponentSystem
//{
//public:
//	EntityComponentSystem() = default;
//	virtual ~EntityComponentSystem() = default;
//public:
//
//	template<typename T, typename... Args>
//	T& Emplace(unsigned int l_id, Args&&... args)
//	{
//		//ASSERT(id, "Entity is not valid");
//		return buffer.emplace<T>(l_id, std::forward<Args>(args)...);
//	}
//
//	template<typename T>
//	T& Get(unsigned int l_id) const
//	{
//		return buffer<T>.at(l_id);
//	}
//
//	template<typename T> void Remove(unsigned int l_id)
//	{
//		buffer.erase(buffer.begin() + l_id);
//	}
//
//private:
//
//};
//
