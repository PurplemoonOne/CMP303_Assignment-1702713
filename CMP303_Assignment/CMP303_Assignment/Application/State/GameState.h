#pragma once
#include "State.h"
#include "../Scene/Scene.h"

#include <memory>

class InputHandler;

class GameState : public State
{
public:
	GameState();
	virtual ~GameState() override;
private:
	virtual void OnStart() override;
	virtual void OnUpdate(float deltaTime, Keyboard* keyboard = nullptr, Gamepad* gamepad = nullptr) override;
	virtual void OnAttach() override;
	virtual void OnDetach() override;
private:


	InputHandler* inputHandler;
};

