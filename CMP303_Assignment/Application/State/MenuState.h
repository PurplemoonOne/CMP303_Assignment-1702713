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

	Entity mStreamButton;
	Entity mSpectateButton;
	sf::Vector2f mScreenDimensions;
};

