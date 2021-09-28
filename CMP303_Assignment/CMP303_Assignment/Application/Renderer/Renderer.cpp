#include "Renderer.h"

Renderer::Renderer(sf::RenderWindow* lwindow)
{
}

Renderer::~Renderer()
{
}

void Renderer::Draw(std::vector<GameObject> objects)
{
}

void Renderer::Draw(std::vector<RendererComponent> objects)
{
	window->clear(sf::Color(1.0f, 0.0f, 0.0f, 1.0f));
	window->setView(sf::View());

	for (auto& component : objects)
	{
	}

}
