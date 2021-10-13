#pragma once
#include "../Components/Components.h"

class Renderer
{
public:
	Renderer() = default;
	Renderer(sf::RenderWindow* lwindow);
	virtual ~Renderer(); 

	void Submit(const std::vector<RendererComponent>& graphics);

private:
	sf::RenderWindow* window;
};

