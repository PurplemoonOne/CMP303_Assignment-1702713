#pragma once
#include <SFML/Graphics.hpp>

class RendererComponent;
class GameObject;
class Renderer
{
public:
	Renderer() = default;
	Renderer(const Renderer&) = default;
	Renderer(sf::RenderWindow* lwindow);
	virtual ~Renderer(); 

	void Draw(std::vector<GameObject> objects);
	void Draw(std::vector<RendererComponent> graphics);

private:

	sf::RenderWindow* window;

};

