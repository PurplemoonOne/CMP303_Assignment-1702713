#include "SpectateState.h"
#include "../Input/Keyboard.h"
#include "../Log/Log.h"

SpectateState::SpectateState(sf::Vector2f screenDimensions)
	:
	mScreenDimensions(screenDimensions),
	mMessageCount(0),
	mCharPressDelay(0.f),
	mPrevCharacter(0)
{
}

SpectateState::~SpectateState()
{
}

void SpectateState::OnStart()
{
	InitHomeButton();
	mCurrentMessage = Entity(mScene, "CurrentMessage");
	mCurrentMessage.GetText().text.setPosition(0 + 128.0f, mScreenDimensions.y - 256.0f);
	mCurrentMessage.GetText().text.setCharacterSize(16.0f);
	mCurrentMessage.GetText().font.loadFromFile("Assets/font.ttf");
	mCurrentMessage.GetText().text.setFont(mCurrentMessage.GetText().font);
	
	mCurrentMessage.GetRenderer().graphics.setPosition(0 + 128.0f, mScreenDimensions.y - 256.0f);
	mCurrentMessage.GetRenderer().graphics.setSize(sf::Vector2f(mScreenDimensions.x * 0.5f, 32.0f));
	mCurrentMessage.GetRenderer().graphics.setFillColor(sf::Color(0, 0, 0, 125));
	mScene->GetClient()->ConnectToServer();
}

void SpectateState::OnUpdate(float deltaTime, const float appElapsedTime, Keyboard* keyboard, Gamepad* gamepad)
{
	//Check if the user has erased a character.
	if (keyboard->IsKeyPressed(sf::Keyboard::Key::BackSpace))
	{
		keyboard->SetKeyUp(sf::Keyboard::Key::BackSpace);
		//Delete the last character in the buffer.
		if (mMessageBuffer.getSize() > 0)
			mMessageBuffer.erase(mMessageBuffer.getSize() - 1, 1);
	}

	//Construct a message from the user input.
	FillMessageBuffer(keyboard->GetRecentCharacter(), deltaTime);

	//Submit the message to the server.
	SubmitMessageToServer(keyboard, appElapsedTime);
	
	//Recieve messages from the server and add them to the registery.
	RecieveMessagesFromServer();

	if (QueryButton(keyboard))
	{
		mScene->GetClient()->Disconnect();
		mScene->TransitionState("menu");
	}

	//Reset character to 0.
	keyboard->SetCharacter(0);
}

void SpectateState::OnDetach()
{
	mScene->GetRegistery()->ClearRegistery();
}

bool SpectateState::QueryButton(Keyboard* keyboard)
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

void SpectateState::InitHomeButton()
{
	mHomeButton = Entity(mScene, "HomeButton");

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

void SpectateState::FillMessageBuffer(char character, const float deltaTime)
{
	char currentCharacter = character;

	//Max buffer size is 128 characters.
	if (mMessageBuffer.getSize() > 128)
	{
		return;
	}

	if (currentCharacter != 0)
	{
		if (currentCharacter == mPrevCharacter)
		{
			if (mCharPressDelay < 1.f)
			{
				mCharPressDelay += 1.f * deltaTime;
				return;
			}
		}
		else
		{
			mCharPressDelay = 0.f;
		}

		mMessageBuffer.insert(mMessageBuffer.getSize(), sf::String(currentCharacter));
		mPrevCharacter = currentCharacter;
	}
	else
	{
		mPrevCharacter = 0;
	}

	mCurrentMessage.GetText().text.setString(mMessageBuffer);
}

void SpectateState::SubmitMessageToServer(Keyboard* keyboard, const float appElapsedTime)
{
	if (keyboard->IsKeyPressed(sf::Keyboard::Key::Enter))
	{
		keyboard->SetKeyUp(sf::Keyboard::Key::Enter);

		if (mCurrentMessage.GetText().text.getString().getSize() != 0)
		{
			//Add a new entity representing the text object to the renderer.
			Entity newMessage = Entity(mScene, "MSG" + std::to_string(mMessageCount));
			newMessage.GetText().text = mCurrentMessage.GetText().text;
			newMessage.GetText().bInit = true;

			//Store the text of the message.
			mMessageLog.push_back(newMessage.GetText().text);

			//Update each text objects position (If the tag is a MSG + ID).
			for (int i = 0; i < mScene->GetRegistery()->GetTagComponents().size(); ++i)
			{
				TagComponent& tag = mScene->GetRegistery()->GetTagComponent(i);

				if (tag.tag == "MSG" + std::to_string(i))
				{
					sf::Vector2f textPos = mScene->GetRegistery()->GetTextComponent(tag.tag).text.getPosition();
					mScene->GetRegistery()->GetTextComponent(tag.tag).text.setPosition(textPos.x, textPos.y - 16.f);
				}
			}

			//Send message to the server.
			if (mScene->GetClient()->SendChatMessage(newMessage.GetText().text))
			{
				mMessageCount++;
			}

			mMessageBuffer.clear();
			mCurrentMessage.GetText().text.setString(mMessageBuffer);
		}
	}
}

void SpectateState::RecieveMessagesFromServer()
{
}
