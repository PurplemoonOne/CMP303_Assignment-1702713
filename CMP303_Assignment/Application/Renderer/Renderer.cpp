#include "Renderer.h"

Renderer::Renderer(sf::RenderWindow* lwindow)
	:
	window(lwindow)
{
}

Renderer::~Renderer()
{
}

void Renderer::Submit(const std::vector<RendererComponent>& objects, const std::vector<TextComponent>& texts)
{
	window->clear(sf::Color(125, 0, 125));

	for (auto& drawable : objects)
	{
		if (drawable.bInit)
		{
			window->draw(drawable.graphics);
		}
	}

	for (auto& text : texts)
	{
		if (text.bInit)
		{
			window->draw(text.text);
		}
	}

	window->display();
}


