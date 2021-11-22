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
	mStates["game"] = new GameState((sf::Vector2f)window->getSize());
	mStates["menu"] = new MenuState((sf::Vector2f)window->getSize());

	//Point to the render window.
	mRenderer = Renderer(window);

	TransitionState("menu");

}

Scene::~Scene()
{  
	if (mStates["game"])
	{
		delete mStates["game"];
		mStates["game"] = nullptr;
	}
	if (mStates["menu"])
	{
		delete mStates["menu"];
		mStates["menu"] = nullptr;
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

	mActiveState = mStates[state];
	mActiveState->SetScene(this);
	mActiveState->OnStart();
}

void Scene::UpdateActiveState(const float time, const float appElapsedTime, Keyboard* keyboard, Gamepad* gamepad, bool debugInfo)
{
	mNetworkTickRate += 1.f * time;

	//Update the current state.
	mActiveState->OnUpdate(time, appElapsedTime, keyboard, gamepad);

	//Render the objects on client side first.
	mRenderer.Submit(mRegistery.GetRendererComponents());

	//Send data to the server.
	if(mActiveState == mStates["game"])
	{
		if (mNetworkTickRate > 0.0125f)
		{
			mNetworkTickRate = 0.f;

			for (sf::Uint32 i = 5; i > 0; --i)
			{
				mClient->SendPacket({ mRegistery.GetTransformComponent(i).position.x, mRegistery.GetTransformComponent(i).position.y }, appElapsedTime, i);
			}
		}
	}
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

