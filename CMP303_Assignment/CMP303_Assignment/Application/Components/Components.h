#pragma once
//#include "../Audio/Audio.h"
//#include "../Graphics/Graphics.h"
#include "../Camera/Camera.h"

#include <string>

//SFML
#include "SFML/System/Vector2.hpp"

struct TagComponent
{
	std::string tag;
	TagComponent(std::string ltag = "Empty")
		:
		tag(ltag){}

	TagComponent(const TagComponent& lTagComponent)
		:
		tag(lTagComponent.tag){}
};

struct Transform
{
	Transform() = default;
	Transform(const Transform&) = default;
	Transform(sf::Vector2f pos, sf::Vector2f rot, sf::Vector2f scl)
		:
		position(pos),
		rotation(rot),
		scale(scl)
	{}

	sf::Vector2f position, rotation, scale;
};

struct TransformComponent
{
	Transform transform;

	TransformComponent()
		:
		transform(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(0.0f, 0.0f), sf::Vector2f(1.0f, 1.0f))
	{}

	TransformComponent(const TransformComponent& transComponent)
		:
		transform(transComponent.transform)
	{}
};

struct CameraComponent
{
	Camera camera;

	CameraComponent() = default;
	CameraComponent(const CameraComponent& camComponent)
		:
		camera(camComponent.camera)
	{}
};

struct RendererComponent
{
	sf::Color colour;
	sf::RectangleShape polygon;

	bool wireframe = false;

	RendererComponent() = default;
	RendererComponent(const RendererComponent& rendererComponent)
	{}
};

struct TextureComponent
{
	sf::Texture texture;
	TextureComponent() = default;
	TextureComponent(const TextureComponent& textureComponent)
		:
		texture(textureComponent.texture)
	{}
};

struct BoxColliderComponent
{
	sf::RectangleShape box;

	sf::Vector2f size;

	BoxColliderComponent() = default;
	BoxColliderComponent(sf::Vector2f lsize) 
	:
		size(lsize)
	{}
	BoxColliderComponent(const BoxColliderComponent& collider)
		:
		box(collider.box)
	{}
};

struct CircleColliderComponent
{
	sf::CircleShape circle;

	float radius;

	CircleColliderComponent() = default;
	CircleColliderComponent(float lradius)
	:
	radius(lradius)
	{}

	CircleColliderComponent(const CircleColliderComponent& collider)
		:
		circle(collider.circle),
		radius(collider.radius)
	{}
};