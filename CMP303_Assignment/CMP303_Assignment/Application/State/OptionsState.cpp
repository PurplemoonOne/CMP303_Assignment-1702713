#include "../pch.h"
#include "../Context/Context.h"
#include "OptionsState.h"

OptionsState::OptionsState()
{
}

OptionsState::~OptionsState()
{
}

void OptionsState::OnStart()
{
}

void OptionsState::OnUpdate(float deltaTime)
{
}

void OptionsState::OnRender()
{
}

void OptionsState::OnAttach()
{
	
}

void OptionsState::OnDetach()
{
	mContext->TransitionState("None");
}
