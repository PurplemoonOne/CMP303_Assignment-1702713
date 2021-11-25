#include "MenuState.h"
#include "../Input/KeyBoard.h"
#include "../Scene/Scene.h"

MenuState::MenuState(sf::Vector2f screenDimensions)
	:
	mScreenDimensions(screenDimensions)
{
	
}

MenuState::~MenuState()
{
}

void MenuState::OnStart()
{
	//Load font
	mStreamButton = Entity(mScene, "StreamButton");
	mSpectateButton = Entity(mScene, "SpectateButton");

	mStreamButton.GetText().font.loadFromFile("Assets/font.ttf");
	mSpectateButton.GetText().font.loadFromFile("Assets/font.ttf");

	//Button text
	mStreamButton.GetText().text.setFont(mStreamButton.GetText().font);
	mStreamButton.GetText().text.setCharacterSize(14);
	mStreamButton.GetText().text.setFillColor(sf::Color::White);
	mStreamButton.GetText().text.setOutlineColor(sf::Color::Black);
	mStreamButton.GetText().text.setOutlineThickness(1.2f);
	mStreamButton.GetText().text.setLetterSpacing(1.5f);
	mStreamButton.GetText().text.setString("Stream");

	mSpectateButton.GetText().text.setFont(mSpectateButton.GetText().font);
	mSpectateButton.GetText().text.setCharacterSize(14);
	mSpectateButton.GetText().text.setFillColor(sf::Color::White);
	mSpectateButton.GetText().text.setOutlineColor(sf::Color::Black);
	mSpectateButton.GetText().text.setOutlineThickness(1.2f);
	mSpectateButton.GetText().text.setLetterSpacing(1.5f);
	mSpectateButton.GetText().text.setString("Spectate");

	//Button graphics
	mStreamButton.GetRenderer().graphics.setFillColor(sf::Color::Black);
	mStreamButton.GetRenderer().graphics.setPosition(mScreenDimensions * 0.5f);
	mStreamButton.GetRenderer().graphics.setSize(sf::Vector2f(128.0f, 128.0f));
	mStreamButton.GetRenderer().graphics.setOrigin(0,0);
	mStreamButton.GetText().text.setPosition(mStreamButton.GetRenderer().graphics.getPosition());

	mSpectateButton.GetRenderer().graphics.setFillColor(sf::Color::Black);
	mSpectateButton.GetRenderer().graphics.setPosition(sf::Vector2f(mScreenDimensions.x * 0.5f, mScreenDimensions.y - 256.f));
	mSpectateButton.GetRenderer().graphics.setSize(sf::Vector2f(128.0f, 128.0f));
	mSpectateButton.GetRenderer().graphics.setOrigin(0, 0);
	mSpectateButton.GetText().text.setPosition(mSpectateButton.GetRenderer().graphics.getPosition());
}

void MenuState::OnUpdate(float deltaTime, const float appElapsedTime, Keyboard* keyboard, Gamepad* gamepad)
{
	static bool bGameState = false;
	static bool bSpectateState = false;
	float mouseX = keyboard->MouseX();
	float mouseY = keyboard->MouseY();

	//Variables to represent button corners.
	sf::Vector2f streamButtonC00 = mStreamButton.GetRenderer().graphics.getPosition();
	sf::Vector2f streamButtonC11 = mStreamButton.GetRenderer().graphics.getPosition() + (mStreamButton.GetRenderer().graphics.getSize());

	sf::Vector2f spectateButtonC00 = mSpectateButton.GetRenderer().graphics.getPosition();
	sf::Vector2f spectateButtonC11 = mSpectateButton.GetRenderer().graphics.getPosition() + (mSpectateButton.GetRenderer().graphics.getSize());

	if ((mouseX > streamButtonC00.x && mouseX < streamButtonC11.x) && (mouseY > streamButtonC00.y && mouseY < streamButtonC11.y))
	{
		mStreamButton.GetRenderer().graphics.setFillColor(sf::Color::Green);
		if (keyboard->MouseLeftButtonDown())
		{
			if (mScene->GetClient() == nullptr)
			{
				mScene->CreateClient(ClientPrivelage::Stream);
			}
			else
			{
				mScene->GetClient()->SetClientPrivelage(ClientPrivelage::Stream);
			}

			bGameState = true;
		}
	}
	else
	{
		mStreamButton.GetRenderer().graphics.setFillColor(sf::Color::Black);
	}
	if ((mouseX > spectateButtonC00.x && mouseX < spectateButtonC11.x) && (mouseY > spectateButtonC00.y && mouseY < spectateButtonC11.y))
	{
		mSpectateButton.GetRenderer().graphics.setFillColor(sf::Color::Green);
		if (keyboard->MouseLeftButtonDown())
		{
			if (mScene->GetClient() == nullptr)
			{
				mScene->CreateClient(ClientPrivelage::Spectate);
			}
			else
			{
				mScene->GetClient()->SetClientPrivelage(ClientPrivelage::Spectate);
			}

			bSpectateState = true;
		}
	}
	else
	{
		mSpectateButton.GetRenderer().graphics.setFillColor(sf::Color::Black);
	}

	if (bGameState)
	{
		bGameState = false;
		mScene->TransitionState("game");
	}
	else if (bSpectateState)
	{
		bSpectateState = false;
		mScene->TransitionState("spectate");
	}

}

void MenuState::OnDetach()
{
	//Clear all the entities that represent the main menu.
	mScene->GetRegistery()->ClearRegistery();
}
