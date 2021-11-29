#include "Entity.h"

Scene* Entity::sScene = nullptr;

Entity::Entity(Scene* scene, const std::string & tag, sf::Texture* texture, size_t index)
	:
	mTag(tag)
{
	sScene = scene;
	sScene->GetRegistery()->AddNewEntity(tag, sf::Vector2f(0.0f, 0.0f), sf::Vector2f(100.0f, 100.0f), texture, index);
}

TransformComponent& Entity::GetTransform()
{
	return sScene->GetRegistery()->GetTransformComponent(mTag);
}

TransformComponent& Entity::GetTransform(const std::string& tag)
{
	return sScene->GetRegistery()->GetTransformComponent(tag);
}

TransformComponent& Entity::GetTransform(const uint16_t id)
{
	return sScene->GetRegistery()->GetTransformComponent(id);
}

RendererComponent& Entity::GetRenderer()
{
	return sScene->GetRegistery()->GetRendererComponent(mTag);
}

RendererComponent& Entity::GetRenderer(const std::string& tag)
{
	return sScene->GetRegistery()->GetRendererComponent(tag);
}

RendererComponent& Entity::GetRenderer(const uint16_t id)
{
	return sScene->GetRegistery()->GetRendererComponent(id);
}

TextComponent& Entity::GetText()
{
	return sScene->GetRegistery()->GetTextComponent(mTag);
}

TextComponent& Entity::GetText(const std::string& tag)
{
	return sScene->GetRegistery()->GetTextComponent(tag);
}

TextComponent& Entity::GetText(const uint16_t id)
{
	return sScene->GetRegistery()->GetTextComponent(id);
}

AnimatorComponent& Entity::GetAnimation(const std::string& tag)
{
	return sScene->GetRegistery()->GetAnimationComponent(tag);
}

TextureComponent& Entity::GetTexture(const std::string& tag)
{
	return sScene->GetRegistery()->GetTextureComponent(tag);
}

