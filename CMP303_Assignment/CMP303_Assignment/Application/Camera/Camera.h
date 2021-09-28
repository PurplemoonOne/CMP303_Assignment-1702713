#pragma once
#include <SFML/Graphics.hpp>

class Camera : public sf::View
{
public:
	Camera() = default;
	Camera(const Camera& lcamera);
	Camera(sf::Vector2f lcentre, sf::Vector2f lsize);
	~Camera();
public:

	void OnUpdate(float deltaTime);

private:
};

