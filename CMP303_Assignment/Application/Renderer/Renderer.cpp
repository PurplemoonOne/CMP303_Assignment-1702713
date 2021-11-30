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

	for (int i = objects.size() - 1; i > -1; --i)
	{
		if (objects.at(i).bShouldRenderSPR)
		{
			window->draw(objects.at(i).sprite);
		}
	}

	for (auto& drawable : objects)
	{
		if (drawable.bShouldRenderGFX)
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


