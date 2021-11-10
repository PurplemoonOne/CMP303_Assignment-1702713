#pragma once
#include <string>
#include "../Scene/Scene.h"

struct Scene;

class Entity
{
public:
	Entity() = default;
	Entity(const Entity&) = default;
	Entity(Scene* scene, const std::string& = "Entity");

	TransformComponent& GetTransform(const std::string& tag);
	RendererComponent& GetRenderer(const std::string& tag);
	AnimatorComponent& GetAnimaton(const std::string& tag);
	TextureComponent& GetTexture(const std::string& tag);

	const std::string& GetTag() const { return mTag; }

private:
	std::string mTag;
	Scene* mScene;
};

