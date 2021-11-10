#include "GameState.h"
#include "../Events/Event.h"
#include "../Input/Keyboard.h"
#include "../Log/Log.h"

GameState::GameState()
{
	connectionManager = new Connection("192.168.0.27");
}

GameState::~GameState()
{}

void GameState::OnStart()
{
	thisEntity = Entity(mScene, "player");
	thisEntity.GetRenderer("player").graphics.setFillColor(sf::Color::Red);

	otherEntity = Entity(mScene, "playerTwo");
	otherEntity.GetRenderer("playerTwo").graphics.setFillColor(sf::Color::Blue);

	//Create our UDP socket
	connectionManager->CreateUDPSocket(5555);

	APP_TRACE("Created new connection on port {0} : Network {1} ", connectionManager->GetSocket().getLocalPort(), "192.168.0.27");

	networkTickRate = 0.f;

}

void GameState::OnUpdate(float deltaTime, const float appElapsedTime, Keyboard* keyboard, Gamepad* gamepad)
{

	networkTickRate += 1.f * deltaTime;

	//Execute the queued event.
	Event* event = inputHandler.HandleKeyboard(keyboard);

	//Check for an input event.
	if (event)
	{
		event->Execute(deltaTime, &thisEntity, keyboard->MouseX(), keyboard->MouseY());
	}

	//Send the data to the server/other client
	if (networkTickRate >= 4.f)
	{
		
		connectionManager->SendPacket
		(
			thisEntity.GetTransform(thisEntity.GetTag()).position.x,
			thisEntity.GetTransform(thisEntity.GetTag()).position.y,
			networkTickRate,
			0,
			4444
		);

		connectionManager->ListenForPacket(4444);

		networkTickRate = 0.f;
	}


	
	
	//Update the position of the sprite on this client's machine.
	mScene->GetRegistery()->UpdateRendererComponent("player");
	mScene->GetRegistery()->UpdateRendererComponent("playerTwo");
}

void GameState::OnAttach()
{
	OnStart();
}

void GameState::OnDetach()
{
	//delete player;

}
