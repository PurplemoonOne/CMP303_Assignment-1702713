#include "pch.h"
#include "Application.h"
#include "Log/Log.h"
#include <iostream>
#include <SFML/Window.hpp>

Application* Application::sApp = nullptr;

Application::Application()
{
	ASSERT(!sApp, "Only one instance of the application can run at one time!");
	sApp = this;

	//Initialise log system.
	Logger::Init();

	//Initialise clock object
	clock = sf::Clock();
}

Application::~Application(){}

void Application::ProcessEvents(sf::RenderWindow& window, Keyboard& keyboard, Scene& scene)
{
	// check all the window's events that were triggered since the last iteration of the loop
	sf::Event event;
	while (window.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::Closed:
			scene.Clean();
			window.close();
			break;
		case sf::Event::Resized:
			window.setSize(sf::Vector2u(event.size.width, event.size.height));
			break;
		case sf::Event::KeyPressed:
			keyboard.SetKeyDown(event.key.code);
			break;
		case sf::Event::KeyReleased:
			keyboard.SetKeyUp(event.key.code);
			break;
		case sf::Event::MouseMoved:
			keyboard.SetMousePos(event.mouseMove.x, event.mouseMove.y);
			break;
		case sf::Event::MouseButtonPressed:
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				keyboard.SetMouseLeftButton(true);
			}
			else if (event.mouseButton.button == sf::Mouse::Right)
			{
				keyboard.SetMouseRightButton(true);
			}
			break;
		case sf::Event::MouseButtonReleased:
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				keyboard.SetMouseLeftButton(false);
			}
			else if (event.mouseButton.button == sf::Mouse::Right)
			{
				keyboard.SetMouseRightButton(false);
			}
			break;
		case sf::Event::TextEntered:
			if (event.text.unicode > 31 && event.text.unicode < 127)
			{
				keyboard.SetCharacter((char)event.text.unicode);
			}
			break;
		default:
			break;
		}
	}
}

void Application::Run()
{
	sf::Time elapsed;
	sf::RenderWindow window(sf::VideoMode(1920, 1080), "303 Assignment");

	Keyboard input;
	Scene scene = Scene(&window);

	float appElapsedTime = 0.f;

	// run the program as long as the window is open
	while (window.isOpen())
	{
		//Process the event queue.
		ProcessEvents(window, input, scene);

		//Calculate delta time.
		elapsed = clock.restart();

		//Update the application's context.
		scene.UpdateActiveState(elapsed.asSeconds(), appElapsedTime, &input, nullptr);

		appElapsedTime += 1.f * elapsed.asSeconds();
	}
}
