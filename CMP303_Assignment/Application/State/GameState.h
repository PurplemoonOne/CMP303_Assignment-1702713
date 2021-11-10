#pragma once
#include "State.h"
#include "../GameObject/Entity.h"
#include "../Input/InputHandler.h"
#include "../Network/Network.h"

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
	virtual void OnAttach() override;
	virtual void OnDetach() override;
private:
	InputHandler inputHandler;
	Entity thisEntity;
	Entity otherEntity;
	float elapsedTime;

private:
	/**	Networking code	**/
	Connection* connectionManager;

	int32_t portNumber;

};

