#pragma once
#include <SFML/Graphics.hpp>

struct RendererComponent
{
	sf::RectangleShape graphics;
};

class Renderer
{
public:
	Renderer() = default;
	Renderer(sf::RenderWindow* lwindow);
	virtual ~Renderer(); 

	//void Draw(std::vector<GameObject> objects);
	void Submit(std::vector<RendererComponent> graphics);

private:
	sf::RenderWindow* window;
};

