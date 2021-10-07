#include "Renderer.h"

Renderer::Renderer(sf::RenderWindow* lwindow)
{
}

Renderer::~Renderer()
{
}

void Renderer::Submit(std::vector<RendererComponent> objects)
{
	window->clear(sf::Color(0, 0, 0));

	for (auto& drawable : objects)
		window->draw(drawable.);

	window->display();
}

