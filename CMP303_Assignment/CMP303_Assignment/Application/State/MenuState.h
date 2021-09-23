#pragma once
#include "State.h"

class MenuState : public State
{
public:
	MenuState();
	virtual ~MenuState() override;
private:
	virtual void OnStart()	override;
	virtual void OnUpdate(float deltaTime) override;
	virtual void OnRender() override;
	virtual void OnAttach() override;
	virtual void OnDetach() override;
private:

};

