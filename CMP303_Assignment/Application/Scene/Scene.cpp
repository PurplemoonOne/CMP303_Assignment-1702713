#include "../pch.h"
#include "../Log/Log.h"
#include "../State/GameState.h"

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

	TransitionState("game");
}

Scene::~Scene()
{

	if (states["game"])
	{
		delete states["game"];
		states["game"] = nullptr;
	}

}

void Scene::TransitionState(std::string state)
{
	//if (states[state] != nullptr)
	//	mActiveState->OnDetach();

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
