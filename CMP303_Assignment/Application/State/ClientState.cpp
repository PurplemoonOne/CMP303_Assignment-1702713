#include "ClientState.h"
#include "../Input/Keyboard.h"
#include "../Log/Log.h"


ClientState::ClientState(sf::Vector2f screenDimensions)
	:
	mScreenDimensions(screenDimensions),
	mHasAssets(false)
{
}

ClientState::~ClientState()
{
}

void ClientState::OnStart()
{
	//Reserve space.
	mScene->GetRegistery()->ReserveBuffer(80);

	//Connect to the server.
	mScene->GetClient()->ConnectToServer();

	//Send client data to server.
	mScene->GetClient()->SendConnectionInformation(1, 1, {128.0f, 128.0f});

	mSwordFishTexture.loadFromFile("Assets/fishGfx/swordfish.png");
	//Create shark.
	mShark = Entity(mScene, "shark", &mSwordFishTexture, 65);
	mShark.GetRenderer().sprite.setScale({ 4.0f, 4.0f });
	mShark.GetRenderer().bShouldRenderGFX = false;
	mShark.GetTransform().scale = { 4.0f, 4.0f };

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
	mScene->GetRegistery()->UpdateSpriteComponent("shark");

	if (QueryButton(keyboard))
	{
		APP_TRACE("Disconnecting from server....");
		if (mScene->GetClient()->Disconnect())
		{
			mHasAssets = false;
			delete mScene->GetClient();
			mScene->TransitionState("menu");
		}
		else
		{
			APP_ERROR("Could not disconnect from the server gracefully.... Terminating program.");
			exit(1);
		}
	}
}

void ClientState::OnDetach()
{
	mScene->GetRegistery()->ClearRegistery();
}

void ClientState::GenerateHostAssets()
{
	//Will wait until it has a valid pack of assets.
	ConnectionData assets = mScene->GetClient()->RecieveAssetsDescFromServer();

	if (assets.count > 0)
	{
		mFishTexture.loadFromFile("Assets/fishGfx/fish.png");
		APP_TRACE("Successfully recieved valid asset descriptions!");

		//Create replicate assets of the host.
		for (sf::Uint32 i = 0; i < assets.count; ++i)
		{
			Entity entity = Entity(mScene, "B" + std::to_string(i), &mFishTexture, i);
			entity.GetRenderer().bShouldRenderGFX = false;
			entity.GetRenderer().sprite.setScale({ 2.f, 2.f });
			entity.GetTransform().scale = { 2.0f, 2.0f };
		}
		mBoidCount = assets.count;
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
	mHomeButton = Entity(mScene, "HomeButton", nullptr, 66);

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
