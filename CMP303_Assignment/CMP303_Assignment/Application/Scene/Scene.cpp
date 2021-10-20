#include "../pch.h"
#include "../Log/Log.h"
#include "../State/GameState.h"
#include "../State/MenuState.h"
#include "../State/OptionsState.h"
#include "../State/PauseState.h"
#include "../State/SplashState.h"
#include "Scene.h"

Scene* Scene::mContext = nullptr;

Scene::Scene()
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
	states["splash"] =  new SplashState();

	mActiveState = states["splash"];
	mActiveState->SetScene(this);

	TransitionState("game");
}

Scene::~Scene()
{

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

	if (states["splash"])
	{
		delete states["splash"];
		states["splash"] = nullptr;
	}
}

void Scene::TransitionState(std::string state)
{
	if (states[state] != nullptr)
		mActiveState->OnDetach();

	mActiveState = states[state];
	mActiveState->SetScene(this);
	mActiveState->OnAttach();
}

void Scene::UpdateActiveState(const float time, sf::RenderWindow* window, Keyboard* keyboard, Gamepad* gamepad)
{
	mActiveState->OnUpdate(time, keyboard, gamepad);
	renderer.Submit(window, registery.GetRendererComponents());
}

void Scene::Clean()
{
	mActiveState->OnDetach();
}
