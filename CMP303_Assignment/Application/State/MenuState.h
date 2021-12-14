#pragma once
#include "State.h"
#include "../GameObject/Entity.h"
#include <SFML\Graphics.hpp>

class MenuState : public State
{
public:
	MenuState(sf::Vector2f screenDimensions);
	~MenuState();
	virtual void OnStart() override;
	virtual void OnUpdate(float deltaTime, const float appElapsedTime, Keyboard* keyboard = nullptr, Gamepad* gamepad = nullptr) override;
	virtual void OnDetach() override;

private:

	void GetInput(Keyboard* keyboard, const float deltaTime);
	bool ValidateIpAddress();
	bool mValidAddress = false;
	float mKeyInputDelay = 0.f;
	sf::String mTextBuffer;
	sf::Font mFont;
	Entity mServerIpText;
	char mLastCharacter;

	void InitBackdrop();
	//Background
	Entity mBackdrop;
	Entity mMiddrop;
	sf::Texture mBackdropTexture;
	sf::Texture mMiddropTexture;

	Entity mHostButton;
	Entity mClientButton;
	sf::Texture mClientButtonTexture;
	sf::Texture mClientButtonPressTexture;
	sf::Texture mHostButtonTexture;
	sf::Texture mHostButtonPressTexture;
	sf::Vector2f mScreenDimensions;

	bool mTexturesLoaded, mFontLoaded;
};
