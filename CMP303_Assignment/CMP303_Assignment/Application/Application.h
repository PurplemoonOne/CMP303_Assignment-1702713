#pragma once
#include "Context/Context.h"

//SFML
#include <SFML/System/Clock.hpp>

namespace sf
{
	class Window;
}

class Application
{
public:
	Application();
	~Application();
public:

	void Run();

private:
	std::unique_ptr<sf::Window> window;
	static Application* sApp;

private:
	std::unique_ptr<Context> context;

	sf::Clock clock;
};

