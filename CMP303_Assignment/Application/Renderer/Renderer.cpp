#include "Renderer.h"

Renderer::Renderer(sf::RenderWindow* lwindow)
	:
	window(lwindow)
{
	font = sf::Font();
	font.loadFromFile("../Assets/Karla-Regular.ttf");
}

Renderer::~Renderer()
{
}

void Renderer::Submit(const std::vector<RendererComponent>& objects)
{
	window->clear(sf::Color(125, 0, 125));

	for (auto& drawable : objects)
	{
  		window->draw(drawable.graphics);
	}

	window->display();
}

void Renderer::DebugRender(uint16_t port, std::string ipa, sf::Vector2f clientPosition, const float appElapsedTime)
{


	sf::Text portText = sf::Text("Port : ", font);
	portText.setPosition(window->getSize().x / 2.f, 0 + 256.f);
	portText.setCharacterSize(11.f);

	sf::Text IPAText = sf::Text("IP Adress : ", font);
	IPAText.setPosition(window->getSize().x / 2.f, 0 + 512.f);
	IPAText.setCharacterSize(11.f);


	sf::Text positionText = sf::Text("Position : ", font);
	positionText.setPosition(window->getSize().x / 2.f, 0 + (512.f + 256.f));
	positionText.setCharacterSize(11.f);


	sf::Text appTime = sf::Text("Elapsed Time : ", font);
	appTime.setPosition(window->getSize().x / 2.f, 0 + (512.f + 512.f));
	appTime.setCharacterSize(11.f);


	window->draw(portText);
	window->draw(IPAText);
	window->draw(positionText);
	window->draw(appTime);
	window->display();

}

