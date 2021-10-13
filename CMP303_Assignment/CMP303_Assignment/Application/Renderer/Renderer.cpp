#include "Renderer.h"

Renderer::Renderer(sf::RenderWindow* lwindow)
	:
	window(lwindow)
{}

Renderer::~Renderer()
{
}

void Renderer::Submit(const std::vector<RendererComponent>& objects)
{
	window->clear(sf::Color(0, 0, 0));

	for (auto& drawable : objects)
		window->draw(drawable.graphics);

	window->display();
}

