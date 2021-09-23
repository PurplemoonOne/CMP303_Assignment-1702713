#pragma once
#include "State.h"

class OptionsState : public State
{
public:
	OptionsState();
	virtual ~OptionsState() override;
private:
	virtual void OnStart()	override;
	virtual void OnUpdate(float deltaTime) override;
	virtual void OnRender() override;
	virtual void OnAttach() override;
	virtual void OnDetach() override;
private:

};

