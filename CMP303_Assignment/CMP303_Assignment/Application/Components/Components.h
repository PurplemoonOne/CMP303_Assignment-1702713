#pragma once
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

#include "../Animation/Animation.h"

struct TagComponent
{
	TagComponent() = default;
	TagComponent(const TagComponent& other) : tag(other.tag) {}
	TagComponent(const std::string& ltag) : tag(ltag) {}
	std::string tag;
};

struct TransformComponent
{
	TransformComponent() = default;
	TransformComponent(const TransformComponent& other, int32_t lid = -1) 
		: 
		position(other.position), 
		scale(other.scale),
		rotation(other.rotation),
		id(lid)
	{}
	TransformComponent(const sf::Vector2f& lposition, float lrotation, const sf::Vector2f& lsize, int32_t lid = -1, const sf::Vector2f& lscale = sf::Vector2f(1.0f, 1.0f))
		:
		position(lposition),
		rotation(lrotation),
		scale(lscale),
		size(lsize),
		id(lid)
	{}
	sf::Vector2f position;
	sf::Vector2f scale; 
	sf::Vector2f size;	//Dimensions
	float rotation;
	int32_t id;
};

struct TextureComponent
{
	TextureComponent() = default;
	TextureComponent(const TextureComponent& other) : texture(other.texture) {}
	TextureComponent(const std::string& fileName, const sf::IntRect& area = sf::IntRect())
	{
		texture.loadFromFile(fileName, area);
		bInit = true;
	}

	sf::Texture texture;
	bool bInit = false;
};


struct AnimatorComponent
{
	AnimatorComponent() = default;
	AnimatorComponent(const AnimatorComponent& other) 
		:
		animation(other.animation),
		bInit(true)
	{}

	Animation animation;
	bool bInit = false;
};

struct RendererComponent
{
	RendererComponent() = default;
	RendererComponent(const RendererComponent& other) 
		:
		graphics(other.graphics),
		bInit(true)
	{}

	RendererComponent(sf::Vector2f position,  sf::Vector2f size, float rotation, sf::Color colour)
	{
		graphics = sf::RectangleShape();
		graphics.setTextureRect(sf::IntRect(0, 1, 100, 100));
		graphics.setOrigin(0.0f, 0.0f);
		graphics.setSize(size);
		graphics.setScale(sf::Vector2f(5.f, 5.f));
		graphics.setPosition(position);
		graphics.setRotation(rotation);
		graphics.setFillColor(colour);
		graphics.setOutlineColor(sf::Color::Blue);
		bInit = true;
	}

	sf::RectangleShape graphics;
	bool bInit = false;
};

struct Registery
{
	// Using a string to access a specific entity.
	std::map<std::string, int32_t> idMap;

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

	// @brief Getters to retrieve specific tags. 

	RendererComponent& GetRendererComponent(std::string tag)
	{
		return graphics[idMap[tag]];
	}

	TransformComponent& GetTransformComponent(std::string tag) 
	{ 
		return transforms[idMap[tag]];
	}

	AnimatorComponent& GetAnimationComponent(std::string tag)
	{ 
		return animations[idMap[tag]]; 
	}

	TextureComponent& GetTextureComponent(std::string tag) 
	{ 
		return textures[idMap[tag]];
	}

	TagComponent& GetTagComponent(std::string tag) 
	{ 
		return tags[idMap[tag]];
	}

	void AddNewEntity(const std::string& tag, sf::Vector2f initialPosition, sf::Vector2f size)
	{
		TagComponent tComp = TagComponent(tag);
		TransformComponent trComp = TransformComponent(initialPosition, 0.0f, size);
		RendererComponent rComp = RendererComponent(initialPosition, size, 0.0f, sf::Color(255, 0, 0));

		tags.push_back(tComp);
		transforms.push_back(trComp);
		graphics.push_back(rComp);
		textures.push_back(TextureComponent());
		animations.push_back(AnimatorComponent());

		transforms.back().id = transforms.size() - 1;
		idMap.emplace(tag, transforms.back().id);
	}

	void UpdateTransformComponent(const std::string& tag, const TransformComponent& transform)
	{
		transforms[idMap[tag]].position = transform.position;
		transforms[idMap[tag]].size = transform.size;
		transforms[idMap[tag]].scale = transform.scale;
	}

	void UpdateRendererComponent(const std::string& tag)
	{
		graphics[idMap[tag]].graphics.setPosition(transforms[idMap[tag]].position);
		graphics[idMap[tag]].graphics.setRotation(transforms[idMap[tag]].rotation);
		graphics[idMap[tag]].graphics.setScale(transforms[idMap[tag]].scale);
	}

	void UpdateRendererComponent(const std::string& tag, const TransformComponent& transform)
	{
		graphics[idMap[tag]].graphics.setPosition(transform.position);
		graphics[idMap[tag]].graphics.setRotation(transform.rotation);
		graphics[idMap[tag]].graphics.setScale(transform.scale);
	}

	void UpdateRendererComponent(const std::string& tag, sf::Vector2f position, sf::Vector2f size, float rotation, sf::Vector2f scale)
	{
		graphics[idMap[tag]].graphics.setPosition(position);
		graphics[idMap[tag]].graphics.setScale(scale);
	}
};