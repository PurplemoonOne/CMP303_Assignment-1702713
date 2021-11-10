#include "Renderer.h"

Renderer::Renderer(sf::RenderWindow* lwindow)
	:
	window(lwindow)
{}

Renderer::~Renderer()
{
}

void Renderer::Submit(sf::RenderWindow* window, const std::vector<RendererComponent>& objects)
{
	window->clear(sf::Color(125, 0, 125));

	for (auto& drawable : objects)
	{
  		window->draw(drawable.graphics);
	}

	window->display();
}

