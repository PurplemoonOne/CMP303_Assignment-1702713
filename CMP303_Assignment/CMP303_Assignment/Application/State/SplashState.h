#pragma once
#include "State.h"

class SplashState : public State
{
public:
	SplashState();
	virtual ~SplashState() override;
private:
	virtual void OnStart()	override;
	virtual void OnUpdate(float deltaTime, Keyboard* keyboard = nullptr, Gamepad* gamepad = nullptr) override;
	virtual void OnAttach() override;
	virtual void OnDetach() override;
};

