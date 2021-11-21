#pragma once
#include "State.h"
#include "../GameObject/Entity.h"
#include "../Input/InputHandler.h"
#include "../Network/Client.h"

#include <memory>

class InputHandler;

class GameState : public State
{
public:
	GameState();
	virtual ~GameState() override;
private:
	virtual void OnStart() override;
	virtual void OnUpdate(float deltaTime, const float appElapsedTime, Keyboard* keyboard = nullptr, Gamepad* gamepad = nullptr) override;
	virtual void OnDetach() override;
private:
	InputHandler inputHandler;
	

	// @brief An object that represents the playable sprite.
	// @note We only need one instance of entity as we manipulate the correct entity 
	// in memory based on the network ID.
	Entity mEntity;
private:

};

