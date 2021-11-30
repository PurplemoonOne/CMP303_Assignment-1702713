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
	mTickUpdateThreshold((1.0f / 64.0f)),
	mLatency(0.f),
	mJitter(0.f)
{
	if (mContext != nullptr)
		ASSERT(!mContext, "Only one application context can exist at once!");

	mContext = this;
	mWindowMaxBoundaries = (sf::Vector2f)window->getSize();
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
			HostNetworking(time, appElapsedTime);
		}
		else if (mActiveState == mStates["client"])
		{
			ClientNetworking(time, appElapsedTime);
		}

		mRenderer.Submit(mRegistery.GetRendererComponents(), mRegistery.GetTextComponents());
	}
}

void Scene::Clean()
{
	if (mClient)
	{
		mClient->Disconnect();
	}
	//Clear the registery.
	mActiveState->OnDetach();
	//Set app to not update to avoid array access errors.
	mActiveState->SetShouldUpdate(false);
}

void Scene::CreateClient(ClientPrivelage privelage)
{
	mClient = new Client(mWindowMaxBoundaries);
	mClient->SetClientPrivelage(privelage);
}

void Scene::HostNetworking(const float deltaTime, const float appElapsedTime)
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
	
	//Recieve packets from the client.
	mClient->RecievePacket();

	if (hasAssets)
	{
		//Get a reference to the game data.
		std::vector<GameData>& gameDataRef = mClient->GetGameData();

		//If we have recieved more than 3 updates...
		if (gameDataRef.size() >= 3)
		{
			static sf::Vector2f predictedPosition;

			//Our lerp from last tick is complete.
			if (mLerpComplete)
			{
				//Set it back to false.
				mLerpComplete = false;

				//Sort our data into order with respect to time sent.
				std::sort(gameDataRef.begin(), gameDataRef.end(), Compare());

				//Calculate the latency for this 'Tick()'.
				mLatency = gameDataRef.at(gameDataRef.size() - 2).time - gameDataRef.at(gameDataRef.size() - 1).time;

				//Run our linear prediction.
				predictedPosition = LinearPrediction(gameDataRef.at(gameDataRef.size() - 1), gameDataRef.at(gameDataRef.size() - 2), 0);
			}
			else
			{
				//Gradually increase the position of each object to the newly predicted.
				if (mLerp < mLerpThreshold)
				{
					mLerp += deltaTime * mLerpSpeed;

					predictedPosition =
					{
						//Linearly interpolate between the last position and new position by 60%
						Lerp(mRegistery.GetTransformComponent("shark").position.x, predictedPosition.x, 0.5f),
						Lerp(mRegistery.GetTransformComponent("shark").position.y, predictedPosition.y, 0.5f)
					};

					mRegistery.GetTransformComponent("shark").position = predictedPosition;
					//Update the new position.
					mRegistery.UpdateSpriteComponent("shark");
				}
				else
				{
					predictedPosition = { 0,0 };
					// Clear messages.
					for (auto& data : gameDataRef)
					{
						//Remove heap allocated data.
						data.DeleteData();
					}
					mClient->GetGameData().clear();
					mClient->GetGameData().resize(0);
					mLerpComplete = true;
					mLerp = 0.0f;
				}
			}
		}
	}
	
}

void Scene::ClientNetworking(const float deltaTime, const float appElapsedTime)
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
			//Obtain the predicted posistion.
			static std::vector<sf::Vector2f> predictedPosition;

			//If our last lerp is complete...
			if (mLerpComplete)
			{
				//Set it back to false.
				mLerpComplete = false;

				//Sort our data into order of time sent.
				std::sort(gameDataRef.begin(), gameDataRef.end(), Compare());

				//Calculate the latency for this 'Tick()'.
				mLatency = gameDataRef.at(gameDataRef.size() - 2).time - gameDataRef.at(gameDataRef.size() - 1).time;

				//For each boid run prediction.
				for (int i = 0; i < boidCount; ++i)
				{
					//Predict position for each boid.
					predictedPosition.push_back(LinearPrediction(gameDataRef.at(gameDataRef.size() - 1), gameDataRef.at(gameDataRef.size() - 2), i));
				}
			}
			else
			{
				//Gradually increase our current position to the newly predicted one.
				if (mLerp < mLerpThreshold)
				{
					mLerp += deltaTime * mLerpSpeed;

					for (int i = 0; i < boidCount; ++i)
					{
						//Interpolate current position
						predictedPosition.at(i) =
						{
							Lerp(mRegistery.GetTransformComponent(i).position.x, predictedPosition.at(i).x, 0.5f),
							Lerp(mRegistery.GetTransformComponent(i).position.y, predictedPosition.at(i).y, 0.5f)
						};

						//Update the new position.
						mRegistery.GetTransformComponent(i).position = predictedPosition.at(i);
						mRegistery.UpdateSpriteComponent(i);
					}
				}
				else
				{
					//Reset our variables.
					predictedPosition.clear();
					predictedPosition.resize(0);
					mLerp = 0.f;
					mLerpComplete = true;
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
	}
	
}

inline sf::Vector2f Scene::LinearPrediction(const GameData& messageA, const GameData& messageB, int index)
{
	float dt = messageA.time - messageB.time;

	float vX = (messageA.x[index] - messageB.x[index]) / dt;
	float vY = (messageA.y[index] - messageB.y[index]) / dt;

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

const float Scene::Lerp(float a, float b, float t)
{
	return (a + t * (b - a));
}
