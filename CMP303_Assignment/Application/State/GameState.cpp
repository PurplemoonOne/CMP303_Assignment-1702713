#include "GameState.h"
#include "../Events/Event.h"
#include "../Input/Keyboard.h"
#include "../Log/Log.h"

GameState::GameState()
{

}

GameState::~GameState()
{
	
}

void GameState::OnStart()
{
	std::string names[] = { "P1", "P2", "P3", "P4", "P5", "P6"};
	sf::Color colours[] = { sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::White, sf::Color::Yellow, sf::Color::Magenta };

	//Push all the player data into the registery class.
	for (uint32_t i = 0; i < 6; ++i)
	{
		Entity entity = Entity(mScene, names[i]);
		entity.GetRenderer(i).graphics.setFillColor(colours[i]);
		entity.GetRenderer(i).graphics.setPosition(sf::Vector2f((float)(rand() % 1000 + 100), (float)(rand() % 1000 + 100)));
		entity.GetRenderer(i).bInit = true;
		if (i == 0)
		{
			mEntity = entity;
		}
	}


	mScene->GetClient()->ConnectToServer();
	//Tell the server about the assets care for prectiction and interpolaiton.
	mScene->GetClient()->SendAssetsToServer({ { AssetType::Rect, 6 } });

}

void GameState::OnUpdate(float deltaTime, const float appElapsedTime, Keyboard* keyboard, Gamepad* gamepad)
{


	//Execute the queued event.
	Event* event = inputHandler.HandleKeyboard(keyboard);

	//Check for an input event.
	if (event)
	{
		//If valid update *our* entity.
		//event->Execute(deltaTime, 0, &mEntity, keyboard->MouseX(), keyboard->MouseY());
	}

	sf::Vector2f mouseCoordinates = sf::Vector2f(keyboard->MouseX(), keyboard->MouseY());

	
	sf::Vector2f forwardVector = mouseCoordinates - mScene->GetRegistery()->GetTransformComponent(0).position;

	//Centre point of the boid.
	sf::Vector2f centroid;

	//Normal to the alignment vector.
	sf::Vector2f normal = sf::Vector2f((cos(90.f) - sin(90.0f)) * forwardVector.x, (sin(90.0f) + cos(90.0f)) * forwardVector.y);

	//Update the player controlled object.
	mScene->GetRegistery()->GetTransformComponent(0).position += forwardVector * 2.f * deltaTime;

	//Calculate the centroid.
	for (int i = 1; i < 6; ++i)
	{
		sf::Vector2f entityPosition = mScene->GetRegistery()->GetTransformComponent(i).position;
		centroid += (entityPosition);
	}
	centroid *= 0.2f;

	//Apply the three rules of a boid - 1. Separation, 2. Alignment, 3. Cohesion
	for (int i = 1; i < 6; ++i)
	{
		sf::Vector2f& entityPosition = mScene->GetRegistery()->GetTransformComponent(i).position;
		sf::Vector2f alignment = mouseCoordinates - entityPosition;
		sf::Vector2f cohesionVector = centroid - entityPosition;
		//Reflection vector
		sf::Vector2f seperationVector = cohesionVector - 2 * (cohesionVector.x * normal.x + cohesionVector.y * normal.y) * normal;
		entityPosition += (seperationVector * 10.0f + alignment * 2.f + cohesionVector * 0.2f) * deltaTime;
	}


	//Update the position of the sprites.
	for(int i = 0; i < 6; ++i)
		mScene->GetRegistery()->UpdateRendererComponent(i);
}

void GameState::OnDetach()
{

}
