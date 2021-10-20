#include "Entity.h"

Entity::Entity(Scene* scene, const std::string & tag)
	:
	mScene(scene),
	mTag(tag)
{
	mScene->GetRegistery()->AddNewEntity(tag, sf::Vector2f(0.0f, 0.0f), sf::Vector2f(1.0f, 1.0f));
}

TransformComponent& Entity::GetTransform(const std::string& tag)
{
	return mScene->GetRegistery()->GetTransformComponent(tag);
}

RendererComponent& Entity::GetRenderer(const std::string& tag)
{
	return mScene->GetRegistery()->GetRendererComponent(tag);
}

AnimatorComponent& Entity::GetAnimaton(const std::string& tag)
{
	return mScene->GetRegistery()->GetAnimationComponent(tag);
}

TextureComponent& Entity::GetTexture(const std::string& tag)
{
	return mScene->GetRegistery()->GetTextureComponent(tag);
}
