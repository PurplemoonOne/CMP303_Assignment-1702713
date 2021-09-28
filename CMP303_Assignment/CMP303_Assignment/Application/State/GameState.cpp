#include "GameState.h"
#include "../Input/InputHandler.h"

#include "../Events/Event.h"


GameState::GameState()
{

}

GameState::~GameState()
{
}

void GameState::OnStart()
{
	
}

void GameState::OnUpdate(float deltaTime, Keyboard* keyboard, Gamepad* gamepad)
{
	inputHandler->HandleKeyboard(keyboard);
	//inputHandler.HandleGamepad(gamepad);

	Event* event = inputHandler->QuedEvent();
	


}

//void GameState::OnRender(sf::RenderWindow* window)
//{
//	window->clear(sf::Color(0.0f, 1.0f, 0.0f));
//	window->setView(sf::View(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(100.0f, 100.0f)));
//
//	window->display();
//}

void GameState::OnAttach()
{
}

void GameState::OnDetach()
{
}
