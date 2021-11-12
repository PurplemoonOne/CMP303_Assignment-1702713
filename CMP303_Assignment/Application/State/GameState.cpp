#include "GameState.h"
#include "../Events/Event.h"
#include "../Input/Keyboard.h"
#include "../Log/Log.h"

GameState::GameState()
{
	mClient = new Client();
}

GameState::~GameState()
{
	if (mClient)
	{
		delete mClient;
		mClient = nullptr;
	}
}

void GameState::OnStart()
{
	mEntity = Entity(mScene, "player");
	mEntity.GetRenderer("player").graphics.setFillColor(sf::Color::Red);

	APP_TRACE("Created new socket on port {0} : Network {1} ", mClient->GetLocalPort(), mClient->GetIPAdress().toString());

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
		event->Execute(deltaTime, &mEntity, keyboard->MouseX(), keyboard->MouseY());
	}

	//Send the data to the server.
	if (networkTickRate >= 4.f)
	{
		APP_TRACE("Sending data to server...");
		mClient->SendPacket
		(
			{mEntity.GetTransform("player").position.x, mEntity.GetTransform("player").position.y},
			appElapsedTime,
			0,
			mClient->GetHostPort()
		);

		//Check to see if the server sends any data to the client.
		mClient->RecievePacket();

		networkTickRate = 0.f;
	}

	//Update the position of the sprite on this client's machine.
	mScene->GetRegistery()->UpdateRendererComponent("player");
}

void GameState::OnAttach()
{
	OnStart();
}

void GameState::OnDetach()
{

}
