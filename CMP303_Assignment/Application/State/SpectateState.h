#pragma once
#include "State.h"
#include "../GameObject/Entity.h"
#include "../Input/InputHandler.h"
#include "../Network/Client.h"

class SpectateState : public State
{
public:
	SpectateState(sf::Vector2f screenDimensions);
	virtual ~SpectateState();

	virtual void OnStart() override;
	virtual void OnUpdate(float deltaTime, const float appElapsedTime, Keyboard* keyboard = nullptr, Gamepad* gamepad = nullptr) override;
	virtual void OnDetach() override;


private:
	bool QueryButton(Keyboard* keyboard);
	void InitHomeButton();

	void FillMessageBuffer(char character, const float deltaTime);
	void SubmitMessageToServer(Keyboard* keyboard, const float appElapsedTime);
	void RecieveMessagesFromServer();

private:
	InputHandler inputHandler;
	sf::Vector2f mScreenDimensions;

	Entity mCurrentMessage;
	std::vector<sf::Text> mMessageLog;

	sf::String mMessageBuffer;
	char mPrevCharacter;
	int mMessageCount;
	float mCharPressDelay;

	Entity mHomeButton;
};