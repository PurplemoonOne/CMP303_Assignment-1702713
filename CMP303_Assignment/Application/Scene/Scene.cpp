#include "../pch.h"
#include "../Log/Log.h"
#include "../State/HostState.h"
#include "../State/MenuState.h"
#include "../State/ClientState.h"

#include "Scene.h"
#include <iostream>

using namespace std::chrono;

struct Compare
{
	inline bool operator() (const GameData& p1, const GameData& p2)
	{
		return p1.systemTime < p2.systemTime;
	}
};

//Send 32 packets in 1 second.
const double TICK_RATE = 1.0f / 64.0f;

Scene* Scene::mContext = nullptr;

Scene::Scene(sf::RenderWindow* window)
	:
	mActiveState(nullptr),
	mTickUpdateThreshold((1.0f / 64.0f)),
	mInitLatencyGraphic(false)
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
	mRegistery.ClearRegistery();
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

		//Create out latency graphics.
		if (!mInitLatencyGraphic)
		{
			if (mActiveState == mStates["host"] || mActiveState == mStates["client"])
			{
				InitLatencyGraphic();
			}
		}
		if (mActiveState == mStates["menu"])
		{
			mInitLatencyGraphic = false;
		}

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

	//Clear the registery.
	mActiveState->OnDetach();
	//Set app to not update to avoid array access errors.
	mActiveState->SetShouldUpdate(false);

	if (mClient)
	{
		mClient->Disconnect();
	}
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

	//Current time.
	mCurrentTimeOnUpdateNetwork = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

	//Delta time since last update.
	mTimeElapsedSinceLastUpdate = mCurrentTimeOnUpdateNetwork - mLastTimeOnUpdateNetwork;

	mTimeElapsedSinceLastUpdate /= 1000.0f;

	//Is the period greater than our defined tick.
	if (mTimeElapsedSinceLastUpdate >= TICK_RATE)
	{
		mLastTimeOnUpdateNetwork = mCurrentTimeOnUpdateNetwork;

		std::vector<sf::Vector2f> positions;
		std::vector<float> rotations;
		std::vector<std::pair<float, float>> scales;
		for (sf::Uint32 i = 0; i < boidCount; ++i)
		{
			//Push back the position, rotation and scale of the object.
			TransformComponent& transform = mRegistery.GetTransformComponent(i);
			positions.push_back(transform.position);
			rotations.push_back(transform.rotation);
			scales.push_back({transform.scale.x, transform.scale.y});
		}

		mClient->SendGamePacket(positions, rotations, scales);
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

			//Set it back to false.
			mLerpComplete = false;

			//Sort our data into order with respect to time sent.
			std::sort(gameDataRef.begin(), gameDataRef.end(), Compare());


			//Run our linear prediction.
			predictedPosition = LinearPrediction(gameDataRef.at(gameDataRef.size() - 1), gameDataRef.at(gameDataRef.size() - 2), 0);
			
			
			predictedPosition =
			{
				//Linearly interpolate between the last position and new position by 60%
				Lerp(mRegistery.GetTransformComponent("shark").position.x, predictedPosition.x, 0.5f),
				Lerp(mRegistery.GetTransformComponent("shark").position.y, predictedPosition.y, 0.5f)
			};

			GameData& recentPacket = gameDataRef.at(gameDataRef.size() - 1);
			mRegistery.GetTransformComponent("shark").position = predictedPosition;
			mRegistery.GetTransformComponent("shark").rotation = recentPacket.rotations[0];
			mRegistery.GetTransformComponent("shark").scale.x = recentPacket.scaleX[0];
			mRegistery.GetTransformComponent("shark").scale.y = recentPacket.scaleY[0];

			//Update the new position.
			mRegistery.UpdateSpriteComponent("shark");
			
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

	//If the other connection has quit remove their assets.
	static_cast<HostState*>(mActiveState)->SetHasAssets(false);

	if (mClient->GetHasClientQuit())
	{
		mRegistery.GetRendererComponent("shark").bShouldRenderSPR = false;
		mRegistery.GetRendererComponent("shark").sprite.setScale(0.f, 0.f);
		mRegistery.GetRendererComponent("shark").sprite.setPosition(0.f, 0.f);
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

	//Current time.
	mCurrentTimeOnUpdateNetwork = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

	//Delta time since last tick.
	mTimeElapsedSinceLastUpdate = mCurrentTimeOnUpdateNetwork - mLastTimeOnUpdateNetwork;

	mTimeElapsedSinceLastUpdate /= 1000.0f;

	//Is our current time elapsed greater than the defined period.
	if (mTimeElapsedSinceLastUpdate >= TICK_RATE)
	{
		mLastTimeOnUpdateNetwork = mCurrentTimeOnUpdateNetwork;

		std::vector<sf::Vector2f> position;
		std::vector<float> rotations;
		std::vector<std::pair<float,float>> scales;

		TransformComponent& transform = mRegistery.GetTransformComponent("shark");

		//Push back the position, rotation and scale of the object.
		position.push_back(transform.position);
		rotations.push_back(transform.rotation);
		scales.push_back({ transform.scale.x, transform.scale.y });
		mClient->SendGamePacket(position, rotations, scales);
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

			
			//Sort our data into order of time sent.
			std::sort(gameDataRef.begin(), gameDataRef.end(), Compare());

			//For each boid run prediction.
			for (int i = 0; i < boidCount; ++i)
			{
				//Predict position for each boid.
				predictedPosition.push_back(LinearPrediction(gameDataRef.at(gameDataRef.size() - 1), gameDataRef.at(gameDataRef.size() - 2), i));
			}


			for (int i = 0; i < boidCount; ++i)
			{
				//Interpolate current position
				predictedPosition.at(i) =
				{
					Lerp(mRegistery.GetTransformComponent(i).position.x, predictedPosition.at(i).x, 0.5f),
					Lerp(mRegistery.GetTransformComponent(i).position.y, predictedPosition.at(i).y, 0.5f)
				};


				GameData& recentPacket = gameDataRef.at(gameDataRef.size() - 1);
				//Update the new position.
				mRegistery.GetTransformComponent(i).position = predictedPosition.at(i);
				mRegistery.GetTransformComponent(i).rotation = recentPacket.rotations[i];
				mRegistery.GetTransformComponent(i).scale.x = recentPacket.scaleX[i];
				mRegistery.GetTransformComponent(i).scale.y = recentPacket.scaleY[i];

				mRegistery.UpdateSpriteComponent(i);
			}
			
			
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

	//If the other connection has quit remove their assets.
	if (mClient->GetHasClientQuit())
	{
		static_cast<ClientState*>(mActiveState)->SetHasAssets(false);
		for (int i = 0; i < boidCount; ++i)
		{
			mRegistery.GetRendererComponent(i).bShouldRenderSPR = false;
			mRegistery.GetRendererComponent(i).sprite.setScale(0.f, 0.f);
			mRegistery.GetRendererComponent(i).sprite.setPosition(0.f, 0.f);
		}
	}
}

inline sf::Vector2f Scene::LinearPrediction(const GameData& messageA, const GameData& messageB, int index)
{
	float dt = messageA.systemTime - messageB.systemTime;
	dt /= 1000.0f;
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
	float dtAB = messageA.systemTime - messageB.systemTime;
	float dtBC = messageB.systemTime - messageC.systemTime;

	dtAB /= 1000.0f;
	dtBC /= 1000.0f;

	float vX = (messageA.x - messageB.x) / dtAB;
	float vY = (messageA.y - messageB.y) / dtAB;
	float aX = (messageA.x - messageB.x) / dtAB - (messageB.x - messageC.x) / dtBC;
	float aY = (messageA.y - messageB.y) / dtAB - (messageB.y - messageC.y) / dtBC;
	float sX = (messageA.x - messageB.x) / dtAB;
	float sY = (messageA.y - messageB.y) / dtAB;

	return { messageA.x[index] + vX * dtAB + 0.5f * aX * dtBC, messageA.y[index] + vY * dtAB + 0.5f * aY * dtBC };
}

const float Scene::Lerp(float a, float b, float t)
{
	return (a + t * (b - a));
}

void Scene::InitLatencyGraphic()
{
	mRegistery.AddNewEntity("Latency", { 0,0 }, { 32.f, 32.f }, 0, 79);
	sf::Text* text = &mRegistery.GetTextComponent("Latency").text;
	sf::RectangleShape* shape = &mRegistery.GetRendererComponent("Latency").graphics;
	mRegistery.GetRendererComponent("Latency").bShouldRenderSPR = false;
	sf::Font font;


	font.loadFromFile("Assets/font.ttf");
	text->setFont(font);
	text->setPosition(128.0f, 128.0f);
	text->setCharacterSize(14.f);
	text->setFillColor(sf::Color::White);
	text->setOutlineColor(sf::Color::Black);
	text->setOutlineThickness(1.2f);
	text->setLetterSpacing(1.5f);
	text->setString("Latency (ms)");

	shape->setFillColor(sf::Color(0, 0, 0, 125));
	shape->setPosition(128.0f, 128.0f);
	mInitLatencyGraphic = true;
}
