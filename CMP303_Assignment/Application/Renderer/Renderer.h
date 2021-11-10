#pragma once
#include "../Components/Components.h"
#include "SFML/Graphics/RenderWindow.hpp"

class Renderer
{
public:
	Renderer() = default;
	Renderer(sf::RenderWindow* lwindow);
	virtual ~Renderer(); 

	void Submit(const std::vector<RendererComponent>& graphics);

	void DebugRender(uint16_t port, std::string ipa, sf::Vector2f clientPosition, const float appElapsedTime);

private:
	sf::RenderWindow* window = nullptr;
	sf::Font font;
};

