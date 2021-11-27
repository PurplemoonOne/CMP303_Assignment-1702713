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

	RendererComponent(sf::Vector2f position, sf::Vector2f size, float rotation, sf::Color colour, sf::Font font = sf::Font(), sf::Text text = sf::Text())
		:
		graphics(size)
	{
		bInit = true;
	}

	sf::RectangleShape graphics;
	sf::Sprite sprite;

	bool bInit = false;
};

struct TextComponent
{
	TextComponent() = default;
	TextComponent(const TextComponent& other)
		:
		font(other.font),
		text(other.text),
		bInit(true)
	{}

	TextComponent(sf::Text& text, sf::Font& font)
		:
		font(font),
		text(text),
		bInit(true)
	{}

	sf::Font font;
	sf::Text text;
	bool bInit = false;
};


struct Registery
{
	// Using a string to access a specific entity.
	std::map<std::string, int32_t> idMap;

	std::vector<TagComponent>       tags;
	std::vector<TransformComponent> transforms;
	std::vector<RendererComponent>  graphics;
	std::vector<TextComponent>		texts;
	std::vector<TextureComponent>   textures;
	std::vector<AnimatorComponent>  animations;

	const std::vector<RendererComponent>& GetRendererComponents() const { return graphics; }
	const std::vector<TextComponent>& GetTextComponents() const { return texts; }
	const std::vector<TransformComponent>& GetTransformComponents() const { return transforms; }
	const std::vector<AnimatorComponent>& GetAnimatorComponents() const { return animations; }
	const std::vector<TextureComponent>& GetTextureComponents() const { return textures; }
	const std::vector<TagComponent>& GetTagComponents() const { return tags; }

	// @brief allocate space in the registery.
	void ReserveBuffer(size_t size)
	{
		graphics.resize(size);
		texts.resize(size);
		tags.resize(size);
		transforms.resize(size);
		textures.resize(size);
		animations.resize(size);
	}

	// @brief Getters to retrieve specific tags. 

	RendererComponent& GetRendererComponent(std::string tag)
	{
		return graphics[idMap[tag]];
	}

	RendererComponent& GetRendererComponent(const uint16_t id)
	{
		return graphics[id];
	}

	TextComponent& GetTextComponent(std::string tag)
	{
		return texts[idMap[tag]];
	}

	TextComponent& GetTextComponent(const uint16_t id)
	{
		return texts[id];
	}

	TransformComponent& GetTransformComponent(std::string tag) 
	{ 
		return transforms[idMap[tag]];
	}

	TransformComponent& GetTransformComponent(const uint16_t id)
	{
		return transforms[id];
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

	TagComponent& GetTagComponent(const uint16_t id)
	{
		return tags[id];
	}

	void ClearRegistery()
	{

		for (auto& tag : tags)
		{
			idMap.erase(tag.tag);
		}

		tags.clear();
		tags.resize(0);
		transforms.clear();
		transforms.resize(0);
		graphics.clear();
		graphics.resize(0);
		textures.clear();
		textures.resize(0);
		texts.clear();
		texts.resize(0);
		animations.clear();
		animations.resize(0);	
		idMap.clear();
	}

	// @brief Insert a new entity into memory.
	void AddNewEntity(const std::string& tag, sf::Vector2f initialPosition, sf::Vector2f size, size_t index)
	{
		TagComponent tComp = TagComponent(tag);
		TransformComponent trComp = TransformComponent(initialPosition, 0.0f, size);
		RendererComponent rComp = RendererComponent(initialPosition, size, 0.0f, sf::Color(255, 0, 0));

		if (index == -1)
		{
			tags.push_back(tComp);
			transforms.push_back(trComp);
			graphics.push_back(rComp);
			textures.push_back(TextureComponent());
			animations.push_back(AnimatorComponent());
			texts.push_back(TextComponent());

			transforms.back().id = transforms.size() - 1;
			idMap.emplace(tag, transforms.back().id);
		}
		else
		{
			//Insert components based on ID.
			tags.at(index) = tComp;
			transforms.at(index) = trComp;
			graphics.at(index) = rComp;
			textures.at(index) = TextureComponent();
			animations.at(index) = AnimatorComponent();
			texts.at(index) = TextComponent();

			transforms.at(index).id = index;
			idMap.emplace(tag, index);
		}
	}
	
	// @brief Update the entity's transform component using a custom tag.
	void UpdateTransformComponent(const std::string& tag, const TransformComponent& transform)
	{
		transforms[idMap[tag]].position = transform.position;
		transforms[idMap[tag]].size = transform.size;
		transforms[idMap[tag]].scale = transform.scale;
	}

	// @brief Update the entity's transform component using it's assigned ID.
	void UpdateTransformComponent(const uint32_t id, const TransformComponent& transform)
	{
		transforms[id].position = transform.position;
		transforms[id].size = transform.size;
		transforms[id].scale = transform.scale;
	}

	// @brief Update the entity's renderer component using a custom tag.
	void UpdateRendererComponent(const std::string& tag)
	{
		graphics[idMap[tag]].graphics.setPosition(transforms[idMap[tag]].position);
		graphics[idMap[tag]].graphics.setRotation(transforms[idMap[tag]].rotation);
		graphics[idMap[tag]].graphics.setScale(transforms[idMap[tag]].scale);
	}

	// @brief Update the entity's renderer component using it's assigned ID.
	void UpdateRendererComponent(const uint32_t id)
	{
  		graphics[id].graphics.setPosition(transforms[id].position);
		graphics[id].graphics.setRotation(transforms[id].rotation);
		graphics[id].graphics.setScale(transforms[id].scale);
	}

	// @brief Update the entity's renderer component using it's assigned ID.
	void UpdateTextComponent(const uint32_t id)
	{
		texts[id].text.setPosition(transforms[id].position);
	}

	// @brief Update the entity's renderer component using it's assigned ID.
	void UpdateTextComponent(const std::string& tag)
	{
		texts[idMap[tag]].text.setPosition(transforms[idMap[tag]].position);
	}

	// @brief Update the entity's renderer component using a custom tag.
	void UpdateRendererComponent(const std::string& tag, const TransformComponent& transform)
	{
		graphics[idMap[tag]].graphics.setPosition(transform.position);
		graphics[idMap[tag]].graphics.setRotation(transform.rotation);
		graphics[idMap[tag]].graphics.setScale(transform.scale);
	}

	// @brief Update the entity's renderer component using it's assigned ID.
	void UpdateRendererComponent(const uint32_t id, const TransformComponent& transform)
	{
		graphics[id].graphics.setPosition(transform.position);
		graphics[id].graphics.setRotation(transform.rotation);
		graphics[id].graphics.setScale(transform.scale);
	}

	// @brief Update the entity's renderer component using a custom tag.
	void UpdateRendererComponent(const std::string& tag, sf::Vector2f position, sf::Vector2f size, float rotation, sf::Vector2f scale)
	{
		graphics[idMap[tag]].graphics.setPosition(position);
		graphics[idMap[tag]].graphics.setScale(scale);
	}

	// @brief Update the entity's renderer component using it's assigned ID.
	void UpdateRendererComponent(const uint32_t id, sf::Vector2f position, sf::Vector2f size, float rotation, sf::Vector2f scale)
	{
		graphics[id].graphics.setPosition(position);
		graphics[id].graphics.setScale(scale);
	}

};