#include "Entity.h"

Entity::Entity(Scene* scene, const std::string & tag, size_t index)
	:
	mScene(scene),
	mTag(tag)
{
	mScene->GetRegistery()->AddNewEntity(tag, sf::Vector2f(0.0f, 0.0f), sf::Vector2f(100.0f, 100.0f), index);
}

TransformComponent& Entity::GetTransform()
{
	return mScene->GetRegistery()->GetTransformComponent(mTag);
}

TransformComponent& Entity::GetTransform(const std::string& tag)
{
	return mScene->GetRegistery()->GetTransformComponent(tag);
}

TransformComponent& Entity::GetTransform(const uint16_t id)
{
	return mScene->GetRegistery()->GetTransformComponent(id);
}

RendererComponent& Entity::GetRenderer()
{
	return mScene->GetRegistery()->GetRendererComponent(mTag);
}

RendererComponent& Entity::GetRenderer(const std::string& tag)
{
	return mScene->GetRegistery()->GetRendererComponent(tag);
}

RendererComponent& Entity::GetRenderer(const uint16_t id)
{
	return mScene->GetRegistery()->GetRendererComponent(id);
}

TextComponent& Entity::GetText()
{
	return mScene->GetRegistery()->GetTextComponent(mTag);
}

TextComponent& Entity::GetText(const std::string& tag)
{
	return mScene->GetRegistery()->GetTextComponent(tag);
}

TextComponent& Entity::GetText(const uint16_t id)
{
	return mScene->GetRegistery()->GetTextComponent(id);
}

AnimatorComponent& Entity::GetAnimation(const std::string& tag)
{
	return mScene->GetRegistery()->GetAnimationComponent(tag);
}

TextureComponent& Entity::GetTexture(const std::string& tag)
{
	return mScene->GetRegistery()->GetTextureComponent(tag);
}

