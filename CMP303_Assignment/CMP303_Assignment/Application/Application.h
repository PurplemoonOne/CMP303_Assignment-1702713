#pragma once
#include "Context/Context.h"
#include "Input/Keyboard.h"
#include "Input/GamePad.h"

//SFML
#include <SFML/System/Clock.hpp>
#include <SFML/System.hpp>

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
	void ProcessEvents(Keyboard& input, Context& context);
private:
	std::unique_ptr<sf::Window> window;
	static Application* sApp;
private:
	sf::Clock clock;
};

