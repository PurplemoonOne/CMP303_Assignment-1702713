#include "MenuState.h"
#include "../Input/KeyBoard.h"
#include "../Scene/Scene.h"

MenuState::MenuState(sf::Vector2f screenDimensions)
	:
	mScreenDimensions(screenDimensions),
	mTexturesLoaded(false),
	mFontLoaded(false)
{
	
}

MenuState::~MenuState()
{
}

void MenuState::OnStart()
{
	//Reserve space.
	mScene->GetRegistery()->ReserveBuffer(80);

	if (!mTexturesLoaded)
	{
		mHostButtonTexture.loadFromFile("Assets/host.png");
		mHostButtonPressTexture.loadFromFile("Assets/hostC.png");

		mClientButtonTexture.loadFromFile("Assets/client.png");
		mClientButtonPressTexture.loadFromFile("Assets/clientC.png");
		mTexturesLoaded = true;
	}


	//Load font
	mHostButton = Entity(mScene, "Host", &mHostButtonTexture, 0);
	mClientButton = Entity(mScene, "Client", &mClientButtonTexture, 1);


	////Button text
	//mHostButton.GetText().text.setFont(mHostButton.GetText().font);
	//mHostButton.GetText().text.setCharacterSize(14);
	//mHostButton.GetText().text.setFillColor(sf::Color::White);
	//mHostButton.GetText().text.setOutlineColor(sf::Color::Black);
	//mHostButton.GetText().text.setOutlineThickness(1.2f);
	//mHostButton.GetText().text.setLetterSpacing(1.5f);
	//mHostButton.GetText().text.setString("Host");

	//mClientButton.GetText().text.setFont(mClientButton.GetText().font);
	//mClientButton.GetText().text.setCharacterSize(14);
	//mClientButton.GetText().text.setFillColor(sf::Color::White);
	//mClientButton.GetText().text.setOutlineColor(sf::Color::Black);
	//mClientButton.GetText().text.setOutlineThickness(1.2f);
	//mClientButton.GetText().text.setLetterSpacing(1.5f);
	//mClientButton.GetText().text.setString("Client");

	//Button graphics
	mHostButton.GetRenderer().graphics.setFillColor(sf::Color(255,255,255,0));
	mHostButton.GetRenderer().graphics.setPosition((mScreenDimensions * 0.5f) - sf::Vector2f(256.0f, 0.f));
	mHostButton.GetRenderer().graphics.setSize(sf::Vector2f(512.0f, 128.0f));
	mHostButton.GetRenderer().graphics.setOrigin(0, 0);

	mHostButton.GetRenderer().sprite.setPosition((mScreenDimensions * 0.5f) - sf::Vector2f(256.0f, 0.f));
	mHostButton.GetRenderer().sprite.setColor(sf::Color(255, 255, 255, 125));
//	mHostButton.GetText().text.setPosition(mHostButton.GetRenderer().graphics.getPosition());

	//Client
	mClientButton.GetRenderer().graphics.setFillColor(sf::Color(255,255,255,0));
	mClientButton.GetRenderer().graphics.setPosition(sf::Vector2f((mScreenDimensions.x * 0.5f) - 256.0f, mScreenDimensions.y - 256.f));
	mClientButton.GetRenderer().graphics.setSize(sf::Vector2f(512.0f, 128.0f));
	mClientButton.GetRenderer().graphics.setOrigin(0, 0);

	mClientButton.GetRenderer().sprite.setPosition(sf::Vector2f((mScreenDimensions.x * 0.5f) - 256.0f, mScreenDimensions.y - 256.f));
	mClientButton.GetRenderer().sprite.setColor(sf::Color(255, 255, 255, 125));
//	mClientButton.GetText().text.setPosition(mClientButton.GetRenderer().graphics.getPosition());

	InitBackdrop();
}

void MenuState::OnUpdate(float deltaTime, const float appElapsedTime, Keyboard* keyboard, Gamepad* gamepad)
{
	static bool bHostState = false;
	static bool bClientState = false;
	float mouseX = keyboard->MouseX();
	float mouseY = keyboard->MouseY();

	//Variables to represent button corners.
	sf::Vector2f streamButtonC00 = mHostButton.GetRenderer().graphics.getPosition();
	sf::Vector2f streamButtonC11 = mHostButton.GetRenderer().graphics.getPosition() + (mHostButton.GetRenderer().graphics.getSize());

	sf::Vector2f spectateButtonC00 = mClientButton.GetRenderer().graphics.getPosition();
	sf::Vector2f spectateButtonC11 = mClientButton.GetRenderer().graphics.getPosition() + (mClientButton.GetRenderer().graphics.getSize());

	//Check if mouse if hovering over button.
	if ((mouseX > streamButtonC00.x && mouseX < streamButtonC11.x) && (mouseY > streamButtonC00.y && mouseY < streamButtonC11.y))
	{
		mHostButton.GetRenderer().sprite.setTexture(mHostButtonPressTexture);
		mHostButton.GetRenderer().sprite.setColor(sf::Color(255,255,255,255));

		if (keyboard->MouseLeftButtonDown())
		{
			mScene->CreateClient(ClientPrivelage::Host);
			bHostState = true;
		}
	}
	else
	{
		mHostButton.GetRenderer().sprite.setTexture(mHostButtonTexture);
		mHostButton.GetRenderer().sprite.setColor(sf::Color(255, 255, 255, 125));
	}

	if ((mouseX > spectateButtonC00.x && mouseX < spectateButtonC11.x) && (mouseY > spectateButtonC00.y && mouseY < spectateButtonC11.y))
	{
		mClientButton.GetRenderer().sprite.setTexture(mClientButtonPressTexture);
		mClientButton.GetRenderer().sprite.setColor(sf::Color(255, 255, 255, 255));
		if (keyboard->MouseLeftButtonDown())
		{
			mScene->CreateClient(ClientPrivelage::Client);
			bClientState = true;
		}
	}
	else
	{
		mClientButton.GetRenderer().sprite.setTexture(mClientButtonTexture);
		mClientButton.GetRenderer().sprite.setColor(sf::Color(255, 255, 255, 125));
	}


	if (bHostState)
	{
		bHostState = false;
		mScene->TransitionState("host");
	}
	else if (bClientState)
	{
		bClientState = false;
		mScene->TransitionState("client");
	}
}

void MenuState::OnDetach()
{
	//Clear all the entities that represent the main menu.
	mScene->GetRegistery()->ClearRegistery();
}

void MenuState::InitBackdrop()
{
	//Load textures
	mBackdropTexture.loadFromFile("Assets/background.png");
	mMiddropTexture.loadFromFile("Assets/midground.png");

	//Mid drop
	mMiddrop = Entity(mScene, "Middrop", &mMiddropTexture, 3);
	mMiddrop.GetRenderer().graphics.setSize({ 0, 0 });
	mMiddrop.GetRenderer().sprite.setPosition(0, 0);
	mMiddrop.GetRenderer().sprite.setScale({ 3,3 });

	//Backdrop
	mBackdrop = Entity(mScene, "Backdrop", &mBackdropTexture, 4);
	mBackdrop.GetRenderer().graphics.setSize({ 0, 0 });
	mBackdrop.GetRenderer().sprite.setPosition(0, 0);
	mBackdrop.GetRenderer().sprite.setScale({ 7,7 });

}
