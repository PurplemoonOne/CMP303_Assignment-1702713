#pragma once
#include "State.h"

class PauseState : public State
{
public:
	PauseState();
	virtual ~PauseState() override;
private:
	virtual void OnStart()	override;
	virtual void OnUpdate(float deltaTime, Keyboard* keyboard = nullptr, Gamepad* gamepad = nullptr) override;
	virtual void OnRender() override;
	virtual void OnAttach() override;
	virtual void OnDetach() override;
private:
};

