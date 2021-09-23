#include "pch.h"
#include "Application.h"
#include "Log/Log.h"

#include <SFML/Window.hpp>


Application* Application::sApp = nullptr;

Application::Application()
{
	ASSERT(!sApp, "Only one instance of the application can run at one time!");
	sApp = this;

	//Initialise log system.
	Logger::Init();

	//Create a window.
	window = std::make_unique<sf::Window>(sf::VideoMode(1920, 1080), "Application");

	//Initialise clock object
	clock = sf::Clock();

	//Initialise the application context
	context = std::make_unique<Context>();
}

Application::~Application()
{
}

void Application::Run()
{

	// run the program as long as the window is open
	while (window->isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window->pollEvent(event))
		{			
			sf::Time elapsed = clock.restart();

			context->UpdateActiveState(elapsed.asSeconds());

			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window->close();
		}
	}
}
