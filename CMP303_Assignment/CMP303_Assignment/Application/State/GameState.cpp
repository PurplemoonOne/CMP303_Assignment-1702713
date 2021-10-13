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

void GameState::OnAttach()
{
}

void GameState::OnDetach()
{
}
