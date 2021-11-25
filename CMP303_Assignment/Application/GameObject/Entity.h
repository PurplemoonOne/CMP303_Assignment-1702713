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

	TransformComponent& GetTransform();
	TransformComponent& GetTransform(const std::string& tag);
	TransformComponent& GetTransform(const uint16_t tag );

	RendererComponent& GetRenderer();
	RendererComponent& GetRenderer(const std::string& tag);
	RendererComponent& GetRenderer(const uint16_t tag    );

	TextComponent& GetText();
	TextComponent& GetText(const std::string& tag);
	TextComponent& GetText(const uint16_t tag);

	AnimatorComponent& GetAnimation(const std::string& tag);

	TextureComponent& GetTexture(const std::string& tag);


	const std::string& GetTag() const { return mTag; }

private:
	std::string mTag;
	Scene* mScene;
};

