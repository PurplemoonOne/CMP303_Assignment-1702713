#pragma once
#include "../Components/Components.h"
#include "SFML/Graphics/RenderWindow.hpp"

class Renderer
{
public:
	Renderer() = default;
	Renderer(sf::RenderWindow* lwindow);
	virtual ~Renderer(); 

	void Submit(sf::RenderWindow* window, const std::vector<RendererComponent>& graphics);

private:
	sf::RenderWindow* window;
};

