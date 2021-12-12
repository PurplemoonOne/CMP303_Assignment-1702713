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
	void InitBackdrop();
	//Background
	Entity mBackdrop;
	Entity mMiddrop;
	sf::Texture mBackdropTexture;
	sf::Texture mMiddropTexture;

	Entity mHostButton;
	sf::Texture mClientButtonTexture;
	sf::Texture mClientButtonPressTexture;
	Entity mClientButton;
	sf::Texture mHostButtonTexture;
	sf::Texture mHostButtonPressTexture;
	sf::Vector2f mScreenDimensions;

	bool mTexturesLoaded, mFontLoaded;
};
