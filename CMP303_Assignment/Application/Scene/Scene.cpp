#include "../pch.h"
#include "../Log/Log.h"
#include "../State/GameState.h"
#include "../State/MenuState.h"

#include "Scene.h"
#include <iostream>

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
	states["menu"] = new MenuState((sf::Vector2f)window->getSize());

	//Point to the render window.
	mRenderer = Renderer(window);

	TransitionState("menu");

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
	if (mClient)
	{
		delete mClient;
		mClient = nullptr;
	}
}

void Scene::TransitionState(std::string state)
{
	if (mActiveState != nullptr)
		mActiveState->OnDetach();

	mActiveState = states[state];
	mActiveState->SetScene(this);
	mActiveState->OnStart();
}

void Scene::UpdateActiveState(const float time, const float appElapsedTime, Keyboard* keyboard, Gamepad* gamepad, bool debugInfo)
{
	mNetworkTickRate += 1.f * time;

	//Update the current state.
	mActiveState->OnUpdate(time, appElapsedTime, keyboard, gamepad);

	//Render the objects on client side first.
	mRenderer.Submit(registery.GetRendererComponents());

}

void Scene::Clean()
{
	mActiveState->OnDetach();
}

void Scene::CreateClient(ClientPrivelage privelage)
{
	mClient = new Client();
	mClient->SetClientPrivelage(privelage);
}

