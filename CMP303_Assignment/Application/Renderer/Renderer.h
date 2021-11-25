#pragma once
#include "../Components/Components.h"
#include "SFML/Graphics/RenderWindow.hpp"

class Renderer
{
public:
	Renderer() = default;
	Renderer(sf::RenderWindow* lwindow);
	virtual ~Renderer(); 

	void Submit(const std::vector<RendererComponent>& graphics, const std::vector<TextComponent>& text);


private:
	sf::RenderWindow* window = nullptr;
};

