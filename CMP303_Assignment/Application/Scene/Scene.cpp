#include "../pch.h"
#include "../Log/Log.h"
#include "../State/HostState.h"
#include "../State/MenuState.h"
#include "../State/ClientState.h"

#include "Scene.h"
#include <iostream>

struct Compare
{
	inline bool operator() (const GameData& p1, const GameData& p2)
	{
		return p1.time < p2.time;
	}
};


Scene* Scene::mContext = nullptr;

Scene::Scene(sf::RenderWindow* window)
	:
	mActiveState(nullptr),
	mTickUpdateThreshold(0.01f)
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

		//Host code.
		if (mActiveState == mStates["host"])
		{
			HostNetworking(appElapsedTime);
		}
		else if (mActiveState == mStates["client"])
		{
			ClientNetworking(appElapsedTime);
		}

		mRenderer.Submit(mRegistery.GetRendererComponents(), mRegistery.GetTextComponents());
	}
}

void Scene::Clean()
{
	mClient->Disconnect();
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

void Scene::HostNetworking(const float appElapsedTime)
{
	//Host sends data about boid.
	sf::Uint32 boidCount = static_cast<HostState*>(mActiveState)->GetBoidCount();

	bool hasAssets = static_cast<HostState*>(mActiveState)->HasAssets();

	if (!hasAssets)
	{
		static_cast<HostState*>(mActiveState)->GenerateClientAssets();
	}

	if (mNetworkTickRate > mTickUpdateThreshold)
	{
		mNetworkTickRate = 0.f;

		std::vector<sf::Vector2f> positions;
		for (sf::Uint32 i = 0; i < boidCount; ++i)
		{
			positions.push_back(mRegistery.GetTransformComponent(i).position);
		}

		mClient->SendGamePacket(positions, appElapsedTime);
	}
	mClient->RecievePacket();

	if (hasAssets)
	{
		if (mClient->GetGameData().size() >= 2)
		{
			//Obtain the predicted posistion. @note The array of positions for the shark is size '1' hence index '0' is passed.
			sf::Vector2f predictedPosition = LinearPrediction(mClient->GetGameData().at(0), mClient->GetGameData().at(1), 0);

			predictedPosition =
			{
				//Linearly interpolate between the last position and new position by 60%
				lerp(mRegistery.GetTransformComponent("shark").position.x, predictedPosition.x, 0.6f),
				lerp(mRegistery.GetTransformComponent("shark").position.y, predictedPosition.y, 0.6f)
			};

			mRegistery.GetTransformComponent("shark").position = predictedPosition;
			//Update the new position.
			mRegistery.UpdateSpriteComponent("shark");

			mClient->GetGameData().clear();
			mClient->GetGameData().resize(0);
		}
	}
}

void Scene::ClientNetworking(const float appElapsedTime)
{
	//Client code	
	bool hasAssets = static_cast<ClientState*>(mActiveState)->HasAssets();

	if (!hasAssets)
	{
		static_cast<ClientState*>(mActiveState)->GenerateHostAssets();
	}

	//Client sends data about shark.
	sf::Uint32 boidCount = static_cast<ClientState*>(mActiveState)->GetBoidCount();

	if (mNetworkTickRate > mTickUpdateThreshold)
	{
		mNetworkTickRate = 0.f;
		std::vector<sf::Vector2f> position;
		position.push_back(mRegistery.GetTransformComponent("shark").position);
		mClient->SendGamePacket(position, appElapsedTime);
	}

	mClient->RecievePacket();

	//If the client has the hosts assets....
	if (hasAssets)
	{
		//Get a reference to the game data.
		std::vector<GameData>& gameDataRef = mClient->GetGameData();

		//For linear we need at least two packets of data to determine position.
		if (gameDataRef.size() >= 3)
		{
			
			std::sort(gameDataRef.begin(), gameDataRef.end(), Compare());
			
			//Obtain the predicted posistion.
			sf::Vector2f predictedPosition;

			//For each boid run prediction.
			for (int i = 0; i < boidCount; ++i)
			{
				//Predict position
				predictedPosition = LinearPrediction(gameDataRef.at(0), gameDataRef.at(1), i);

				//Interpolate current position
				predictedPosition = 
				{
						lerp(mRegistery.GetTransformComponent(i).position.x, predictedPosition.x, 0.6f),
						lerp(mRegistery.GetTransformComponent(i).position.y, predictedPosition.y, 0.6f)
				};
				//Update the new position.
				mRegistery.GetTransformComponent(i).position = predictedPosition;
				mRegistery.UpdateSpriteComponent(i);
			}


			// Clear messages.
			for (auto& data : gameDataRef)
			{
				//Remove heap allocated data.
				data.DeleteData();
			}

			gameDataRef.resize(0);
		}
	}
}

inline sf::Vector2f Scene::LinearPrediction(const GameData& messageA, const GameData& messageB, int index)
{
	float dt = messageB.time - messageA.time;

	float vX = (messageB.x[index] - messageA.x[index]) / dt;
	float vY = (messageB.y[index] - messageA.y[index]) / dt;

	return 
	{
		messageB.x[index] + vX * dt,
		messageB.y[index] + vY * dt 
	};
}

inline sf::Vector2f Scene::QuadraticPrediction(const GameData& messageA, const GameData& messageB, const GameData& messageC, int index)
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

