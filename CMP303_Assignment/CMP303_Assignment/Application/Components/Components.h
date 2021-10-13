#pragma once
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

#include "../Animation/Animation.h"

struct TagComponent
{
	TagComponent() = default;
	TagComponent(const TagComponent& other) : tag(other.tag) {}
	TagComponent(const std::string& ltag) : tag(ltag){}
	std::string tag;
};

struct TransformComponent
{
	TransformComponent() = default;
	TransformComponent(const TransformComponent& other) : position(other.position), scale(other.scale), rotation(other.rotation) {}
	TransformComponent(const sf::Vector2f& lposition, float lrotation, const sf::Vector2f& lscale) : position(lposition), rotation(lrotation), scale(lscale) {}
	sf::Vector2f position;
	sf::Vector2f scale; 
	sf::Vector2f size;	//Dimensions
	float rotation;
};

struct TextureComponent
{
	TextureComponent() = default;
	TextureComponent(const TextureComponent& other) : texture(other.texture) {}
	TextureComponent(const std::string& fileName, const sf::IntRect& area = sf::IntRect())
	{	texture.loadFromFile(fileName, area);	}

	sf::Texture texture;

	bool bInit = false;
};


struct AnimatorComponent
{
	AnimatorComponent() = default;
	AnimatorComponent(const AnimatorComponent& other) : animation(other.animation)
	{}

	Animation animation;

	bool bInit = false;
};

struct RendererComponent
{
	RendererComponent() = default;
	RendererComponent(const RendererComponent& other) : graphics(other.graphics) {}
	RendererComponent(const sf::Vector2f& position, const sf::Vector2f& size, float rotation = 0.0f, const sf::Vector2f& scale = sf::Vector2f(1.0f, 1.0f))
	{
		graphics.setScale(scale);
		graphics.setSize(size);
		graphics.setPosition(position);
		graphics.setRotation(rotation);
	}
	sf::RectangleShape graphics;
	bool bInit = false;
};

struct Registery
{
	std::vector<TagComponent> tags;
	std::vector<TransformComponent> transforms;
	std::vector<RendererComponent> graphics;
	std::vector<TextureComponent> textures;
	std::vector<AnimatorComponent> animations;

	const std::vector<RendererComponent>& GetRendererComponents() const { return graphics; }
	const std::vector<TransformComponent>& GetTransformComponents() const { return transforms; }
	const std::vector<AnimatorComponent>& GetAnimatorComponents() const { return animations; }
	const std::vector<TextureComponent>& GetTextureComponents() const { return textures; }
	const std::vector<TagComponent>& GetTagComponents() const { return tags; }

	RendererComponent& GetRendererComponent(uint32_t id) { return graphics[id]; }
	TransformComponent& GetTransformComponent(uint32_t id) { return transforms[id]; }
	AnimatorComponent& GetAnimationComponent(uint32_t id) { return animations[id]; }
	TextureComponent& GetTextureComponent(uint32_t id) { return textures[id]; }
	TagComponent& GetTagComponent(uint32_t id) { return tags[id]; }

	void UpdateTransformComponent(uint32_t id, const TransformComponent& transform)
	{
		transforms[id].position = transform.position;
		transforms[id].size = transform.size;
		transforms[id].scale = transform.scale;
	}

	void UpdateRendererComponent(uint32_t id)
	{
		graphics[id].graphics.setPosition(transforms[id].position);
		graphics[id].graphics.setSize(transforms[id].size);
		graphics[id].graphics.setRotation(transforms[id].rotation);
		graphics[id].graphics.setScale(transforms[id].scale);
	}

	void UpdateRendererComponent(uint32_t id, const TransformComponent& transform)
	{
		graphics[id].graphics.setPosition(transform.position);
		graphics[id].graphics.setSize(transform.size);
		graphics[id].graphics.setRotation(transform.rotation);
		graphics[id].graphics.setScale(transform.scale);
	}

	void UpdateRendererComponent(uint32_t id, sf::Vector2f position, sf::Vector2f size, float rotation, sf::Vector2f scale)
	{
		graphics[id].graphics.setPosition(position);
		graphics[id].graphics.setSize(size);
		graphics[id].graphics.setScale(scale);
	}
};