#include "GameState.h"
#include "../Events/Event.h"
#include "../Input/Keyboard.h"
#include "../Log/Log.h"

GameState::GameState()
{}

GameState::~GameState()
{}

void GameState::OnStart()
{
	entity = Entity(mScene, "player");
	entity.GetRenderer("player").graphics.setFillColor(sf::Color::Red);

	elapsedTime = 0.f;



}

void GameState::OnUpdate(float deltaTime, Keyboard* keyboard, Gamepad* gamepad)
{
	//Execute the queued event.
	Event* event = inputHandler.HandleKeyboard(keyboard);

	//Check for an input event.
	if (event)
	{
		event->Execute(deltaTime, &entity, keyboard->MouseX(), keyboard->MouseY());
	}

	//Send the data to the server/other client

	
	//Update the position of the sprite on this client's machine.
	mScene->GetRegistery()->UpdateRendererComponent("player");
}

void GameState::OnAttach()
{
	OnStart();
}

void GameState::OnDetach()
{
	//delete player;

	// Unbind socket.
	sendSocket.unbind();
}
