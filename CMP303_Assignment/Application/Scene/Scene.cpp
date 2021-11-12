#include "../pch.h"
#include "../Log/Log.h"
#include "../State/GameState.h"

#include "Scene.h"

Scene* Scene::mContext = nullptr;

Scene::Scene(sf::RenderWindow* window)
	:
	mActiveState(nullptr)
{
	if (mContext != nullptr)
		ASSERT(!mContext, "Only one application context can exist at once!");

	mContext = this;

	//Add States
	states["game"] = new GameState();

	TransitionState("game");

	mRenderer = Renderer(window);
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

void Scene::UpdateActiveState(const float time, const float appElapsedTime, Keyboard* keyboard, Gamepad* gamepad, bool debugInfo)
{
	//Update the current state.
	mActiveState->OnUpdate(time, appElapsedTime, keyboard, gamepad);

	//Submit all of the updated sprites to the renderer.
	mRenderer.Submit(registery.GetRendererComponents());
}

void Scene::Clean()
{
	mActiveState->OnDetach();
}
