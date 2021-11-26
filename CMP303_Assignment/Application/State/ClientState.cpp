#include "ClientState.h"
#include "../Input/Keyboard.h"
#include "../Log/Log.h"


ClientState::ClientState(sf::Vector2f screenDimensions)
	:
	mScreenDimensions(screenDimensions)
{
}

ClientState::~ClientState()
{
}

void ClientState::OnStart()
{
	//Reserve space.
	mScene->GetRegistery()->ReserveBuffer(260);
	//Connect to the server.
	mScene->GetClient()->ConnectToServer();

	if (!mHasAssets)
		GenerateHostAssets();

	//Create shark.
	mShark = Entity(mScene, "shark", 129);
	mShark.GetRenderer().graphics.setFillColor(sf::Color::Green);
	mShark.GetRenderer().graphics.setSize(sf::Vector2f(128.0f, 128.0f));
	mHasAssets = true;

	InitHomeButton();
}

void ClientState::OnUpdate(float deltaTime, const float appElapsedTime, Keyboard* keyboard, Gamepad* gamepad)
{
	//Execute the queued event.
	Event* event = inputHandler.HandleKeyboard(keyboard);

	//Check for an input event.
	if (event)
	{
		//If valid update *our* entity.
		event->Execute(deltaTime, &mShark, keyboard->MouseX(), keyboard->MouseY());
	}

	//Update the position of the sprites.
	mScene->GetRegistery()->UpdateRendererComponent("shark");

	if (QueryButton(keyboard))
	{
		APP_TRACE("Disconnecting from server....");
		mScene->GetClient()->Disconnect();
		delete mScene->GetClient();
		mScene->TransitionState("menu");
	}
}

void ClientState::OnDetach()
{
	mScene->GetRegistery()->ClearRegistery();
}

void ClientState::GenerateHostAssets()
{
	//Will wait until it has a valid pack of assets.
	ConnectionData assets = mScene->GetClient()->RecieveHostAssets();

	if (assets.count < 256)
	{
		//Create replicate assets of the host.
		for (sf::Uint32 i = 0; i < assets.count; ++i)
		{
			Entity entity = Entity(mScene, "B" + std::to_string(i));
			entity.GetRenderer().graphics.setFillColor(sf::Color::Red);
			entity.GetRenderer().graphics.setSize(sf::Vector2f(assets.sizeX, assets.sizeY));
		}

		mHasAssets = true;
	}
}

bool ClientState::QueryButton(Keyboard* keyboard)
{
	static bool bButtonPressed = false;
	float mouseX = keyboard->MouseX();
	float mouseY = keyboard->MouseY();

	//Variables to represent button corners.
	sf::Vector2f homeButtonC00 = mHomeButton.GetRenderer().graphics.getPosition();
	sf::Vector2f homeButtonC11 = mHomeButton.GetRenderer().graphics.getPosition() + (mHomeButton.GetRenderer().graphics.getSize());

	if ((mouseX > homeButtonC00.x && mouseX < homeButtonC11.x) && (mouseY > homeButtonC00.y && mouseY < homeButtonC11.y))
	{
		mHomeButton.GetRenderer().graphics.setFillColor(sf::Color::Green);
		if (keyboard->MouseLeftButtonDown())
		{
			bButtonPressed = true;
		}
	}
	else
	{
		mHomeButton.GetRenderer().graphics.setFillColor(sf::Color::Black);
		bButtonPressed = false;
	}

	return bButtonPressed;
}

void ClientState::InitHomeButton()
{
	mHomeButton = Entity(mScene, "HomeButton", 130);

	mHomeButton.GetText().font.loadFromFile("Assets/font.ttf");

	//Button text
	mHomeButton.GetText().text.setFont(mHomeButton.GetText().font);
	mHomeButton.GetText().text.setCharacterSize(14);
	mHomeButton.GetText().text.setFillColor(sf::Color::White);
	mHomeButton.GetText().text.setOutlineColor(sf::Color::Black);
	mHomeButton.GetText().text.setOutlineThickness(1.2f);
	mHomeButton.GetText().text.setLetterSpacing(1.5f);
	mHomeButton.GetText().text.setString("Home");

	mHomeButton.GetRenderer().graphics.setFillColor(sf::Color::Black);
	mHomeButton.GetRenderer().graphics.setPosition(mScreenDimensions * 0.8f);
	mHomeButton.GetRenderer().graphics.setSize(sf::Vector2f(128.0f, 128.0f));
	mHomeButton.GetRenderer().graphics.setOrigin(0, 0);
	mHomeButton.GetText().text.setPosition(mHomeButton.GetRenderer().graphics.getPosition());
}
