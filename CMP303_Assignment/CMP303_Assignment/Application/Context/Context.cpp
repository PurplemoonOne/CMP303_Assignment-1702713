#include "../pch.h"
#include "../Log/Log.h"
#include "../State/GameState.h"
#include "../State/MenuState.h"
#include "../State/OptionsState.h"
#include "../State/PauseState.h"
#include "Context.h"

Context* Context::mContext = nullptr;

Context::Context()
	:
	mActiveState(nullptr)
{
	if (mContext != nullptr)
		ASSERT(!mContext, "Only one application context can exist at once!");

	mContext = this;
	//Add States
	states["game"] =	new GameState();
	states["menu"] =	new MenuState();
	states["options"] = new OptionsState();
	states["pause"] =   new PauseState();

	TransitionState("splash");
}

Context::~Context()
{
	if (mActiveState)
	{
		delete mActiveState;
		mActiveState = nullptr;
	}

	if (states["game"])
	{
		delete states["game"];
		states["game"] = nullptr;
	}

	if (states["menu"])
	{
		delete states["menu"];
		states["menu"] = nullptr;
	}

	if (states["options"])
	{
		delete states["options"];
		states["options"] = nullptr;
	}

	if (states["pause"])
	{
		delete states["pause"];
		states["pause"] = nullptr;
	}
}

void Context::TransitionState(std::string state)
{
	if (states[state] != nullptr)
		mActiveState->OnDetach();

	this->mActiveState = states[state];
	this->mActiveState->SetContext(this);
	this->mActiveState->OnAttach();
}

void Context::UpdateActiveState(float time)
{
	this->mActiveState->OnUpdate(time);
	this->mActiveState->OnRender();
}
