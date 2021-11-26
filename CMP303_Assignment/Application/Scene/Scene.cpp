#include "../pch.h"
#include "../Log/Log.h"
#include "../State/HostState.h"
#include "../State/MenuState.h"
#include "../State/ClientState.h"

#include "Scene.h"
#include <iostream>


//Custom sort functor for sorting packets into ID order.
class GamePacketIDSort
{
public:
	inline bool operator()(const GameData& p1, const GameData& p2, int a, int b)
	{
		return (p1.objectIDs[a] < p2.objectIDs[b]);
	}
};

Scene* Scene::mContext = nullptr;

Scene::Scene(sf::RenderWindow* window)
	:
	mActiveState(nullptr)
{
	if (mContext != nullptr)
		ASSERT(!mContext, "Only one application context can exist at once!");

	mContext = this;

	//Add States
	mStates["host"] = new HostState((sf::Vector2f)window->getSize());
	mStates["client"] = new ClientState((sf::Vector2f)window->getSize());
	mStates["menu"] = new MenuState((sf::Vector2f)window->getSize());

	//Point to the render window.
	mRenderer = Renderer(window);

	TransitionState("menu");

}

Scene::~Scene()
{  
	if (mStates["host"])
	{
		delete mStates["host"];
		mStates["host"] = nullptr;
	}

	if (mStates["client"])
	{
		delete mStates["client"];
		mStates["client"] = nullptr;
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
	//Checks if the application should update.
	if (mActiveState->Update())
	{

		mNetworkTickRate += 1.f * time;

		//Update the current state.
		if (mActiveState != nullptr)
			mActiveState->OnUpdate(time, appElapsedTime, keyboard, gamepad);


		//Render the objects on client side first.
		mRenderer.Submit(mRegistery.GetRendererComponents(), mRegistery.GetTextComponents());

		//Check for new ports.
		//if(mClient)
		//	mClient->GatherNewPorts(); 

		//Send data to the server.
		//Host code.
		if (mActiveState == mStates["host"])
		{
			//Host sends data about boid.
			sf::Uint32 boidCount = static_cast<HostState*>(mActiveState)->GetBoidCount();
			if (mNetworkTickRate > 0.0125f)
			{
				mNetworkTickRate = 0.f;
				std::vector<sf::Vector2f> positions;
				for (sf::Uint32 i = 0; i < boidCount; ++i)
				{
					positions.push_back(mRegistery.GetTransformComponent(i).position);
				}
				mClient->SendGamePacket(positions.data());
			}

			mClient->RecievePacket();

			if (mClient->GetGameData().size() > 2)
			{
				//Obtain the predicted posistion.
				sf::Vector2f predictedPosition = LinearPrediction(mClient->GetGameData().at(0), mClient->GetGameData().at(1));

				sf::Vector2f newPosition = sf::Vector2f
				(
					//Linearly interpolate between the last position and new position by 60%
					lerp(mRegistery.GetTransformComponent("shark").position.x, predictedPosition.x, 0.6f),
					lerp(mRegistery.GetTransformComponent("shark").position.y, predictedPosition.y, 0.6f)
				);

				//Update the new position.
				mRegistery.GetRendererComponent("shark").graphics.setPosition(newPosition);
			}
		}

		//Client code
		else if (mActiveState == mStates["client"])
		{
			bool hasAssets = static_cast<ClientState*>(mActiveState)->HasAssets();

			if (!hasAssets)
			{
				static_cast<ClientState*>(mActiveState)->GenerateHostAssets();
			}

			//Client sends data about shark.
			sf::Uint32 boidCount = static_cast<ClientState*>(mActiveState)->GetBoidCount();

			if (mNetworkTickRate > 0.0125f)
			{
				mNetworkTickRate = 0.f;
				sf::Vector2f position = mRegistery.GetTransformComponent("shark").position;
				mClient->SendGamePacket(&position);
			}

			if (hasAssets)
			{
				mClient->RecievePacket();

				//For linear we need at least two packets of data to determine position.
				//After (recieving 2 * numberOfEntities) we can begin to predict new positions.
				int offset = boidCount;
				for (int i = 0; i < boidCount; ++i)
				{
					if ((i + offset) > (boidCount * 2))
					{
						return;
					}
					else
					{
						//Obtain the predicted posistion.
						sf::Vector2f predictedPosition = LinearPrediction(mClient->GetGameData().at(i), mClient->GetGameData().at(i + offset));

						sf::Vector2f newPosition = sf::Vector2f
						(
							//Linear interpolate between the last position and new position by 60%
							lerp(mRegistery.GetTransformComponent(i).position.x, predictedPosition.x, 0.6f),
							lerp(mRegistery.GetTransformComponent(i).position.y, predictedPosition.y, 0.6f)
						);

						//Update the new position.
						mRegistery.GetRendererComponent(i).graphics.setPosition(newPosition);
					}
				}
			}
		}

		//Second draw using new interpolated methods.
		mRenderer.Submit(mRegistery.GetRendererComponents(), mRegistery.GetTextComponents());
	}
}

void Scene::Clean()
{
	//Clear the registery.
	mActiveState->OnDetach();

	//Set app to not update to avoid array access errors.
	mActiveState->SetShouldUpdate(false);
}

void Scene::CreateClient(ClientPrivelage privelage)
{
	mClient = new Client();
	mClient->SetClientPrivelage(privelage);
}

inline sf::Vector2f Scene::LinearPrediction(const GameData& messageA, const GameData& messageB)
{
	float dt = messageA.time - messageB.time;

	float vX = (messageA.x - messageB.x) / dt;
	float vY = (messageA.y - messageB.y) / dt;

	return { *messageA.x + vX * dt, *messageA.y + vY * dt };
}

inline sf::Vector2f Scene::QuadraticPrediction(const GameData& messageA, const GameData& messageB, const GameData& messageC)
{
	float dtAB = messageA.time - messageB.time;
	float dtBC = messageB.time - messageC.time;

	float vX = (messageA.x - messageB.x) / dtAB;
	float vY = (messageA.y - messageB.y) / dtAB;
	float aX = (messageA.x - messageB.x) / dtAB - (messageB.x - messageC.x) / dtBC;
	float aY = (messageA.y - messageB.y) / dtAB - (messageB.y - messageC.y) / dtBC;
	float sX = (messageA.x - messageB.x) / dtAB;
	float sY = (messageA.y - messageB.y) / dtAB;

	return { *messageA.x + vX * dtAB + 0.5f * aX * dtBC, *messageA.y + vY * dtAB + 0.5f * aY * dtBC };
}

