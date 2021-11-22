#include "GameState.h"
#include "../Events/Event.h"
#include "../Input/Keyboard.h"
#include "../Log/Log.h"

GameState::GameState(sf::Vector2f screenDimensions)
	:
	mScreenDimensions(screenDimensions)
{

}

GameState::~GameState()
{
	
}

void GameState::OnStart()
{
	//Initialise the boid graphics.
	const std::string names[] = { "P1", "P2", "P3", "P4", "P5", "P6"};
	const sf::Color colours[] = { sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::White, sf::Color::Yellow, sf::Color::Magenta };

	//Push all the player data into the registery class.
	for (uint32_t i = 0; i < 6; ++i)
	{
		Entity entity = Entity(mScene, names[i]);
		entity.GetRenderer(i).graphics.setFillColor(colours[i]);
		entity.GetRenderer(i).graphics.setPosition(0.f, 0.f);
		entity.GetRenderer(i).bInit = true;

		mBoids.push_back(entity);
	}

	InitHomeButton();

	mScene->GetClient()->ConnectToServer();


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
	
	//Calculate the seperation vectors
	float desiredSeperation = 25.0f;
	sf::Vector2f steer = sf::Vector2f(0, 0);
	int count = 0;

	for (auto& boid : mBoids)
	{
		
	}





	//Update the position of the sprites.
	for(int i = 0; i < 6; ++i)
		mScene->GetRegistery()->UpdateRendererComponent(i);


	if (QueryButton(keyboard))
	{
		mScene->GetClient()->Disconnect(appElapsedTime);
		mScene->TransitionState("menu");
	}
}

void GameState::OnDetach()
{
	mScene->GetRegistery()->ClearRegistery();
}

bool GameState::QueryButton(Keyboard* keyboard)
{
	static bool bButtonPressed = false;
	float mouseX = keyboard->MouseX();
	float mouseY = keyboard->MouseY();

	//Variables to represent button corners.
	sf::Vector2f homeButtonC00 = mHomeButton.GetRenderer().graphics.getPosition();
	sf::Vector2f homeButtonC11 = mHomeButton.GetRenderer().graphics.getPosition() + (mHomeButton.GetRenderer().graphics.getSize());

	if ((mouseX > homeButtonC00.x && mouseX < homeButtonC11.x) && (mouseY > homeButtonC00.y && mouseY < homeButtonC11.y))
	{
		mHomeButton.GetRenderer().graphics.setFillColor(sf::Color::Green);
		if (keyboard->MouseLeftButtonDown())
		{
			bButtonPressed = true;
		}
	}
	else
	{
		mHomeButton.GetRenderer().graphics.setFillColor(sf::Color::Black);
		bButtonPressed = false;
	}

	return bButtonPressed;
}

void GameState::InitHomeButton()
{
	mHomeButton = Entity(mScene, "HomeButton");

	mHomeButton.GetRenderer().font.loadFromFile("Assets/font.ttf");

	//Button text
	mHomeButton.GetRenderer().text.setFont(mHomeButton.GetRenderer().font);
	mHomeButton.GetRenderer().text.setCharacterSize(14);
	mHomeButton.GetRenderer().text.setFillColor(sf::Color::White);
	mHomeButton.GetRenderer().text.setOutlineColor(sf::Color::Black);
	mHomeButton.GetRenderer().text.setOutlineThickness(1.2f);
	mHomeButton.GetRenderer().text.setLetterSpacing(1.5f);
	mHomeButton.GetRenderer().text.setString("Home");

	mHomeButton.GetRenderer().graphics.setFillColor(sf::Color::Black);
	mHomeButton.GetRenderer().graphics.setPosition(mScreenDimensions * 0.8f);
	mHomeButton.GetRenderer().graphics.setSize(sf::Vector2f(128.0f, 128.0f));
	mHomeButton.GetRenderer().graphics.setOrigin(0, 0);
	mHomeButton.GetRenderer().text.setPosition(mHomeButton.GetRenderer().graphics.getPosition());
}

sf::Vector2f Boid::Normalise(sf::Vector2f vector)
{
	float length = 0.f;
	length = sqrt((vector.x * vector.x) + (vector.y * vector.y));
	sf::Vector2f norm;
	norm = vector / length;
	return norm;
}

sf::Vector2f Boid::Seperate(std::vector<Entity>& boids)
{
	float seperationFactor = 25.0f;
	sf::Vector2f steer = sf::Vector2f(0.f, 0.f);
	int count = 0;

	for (auto& boid : boids)
	{
		sf::Vector2f difference = mEntity.GetTransform().position - boid.GetTransform().position;
		float distance = sqrtf(difference.x * difference.x + difference.y * difference.y);

		if ((distance > 0) && (distance < seperationFactor))
		{
			difference = Normalise(difference);
			difference /= distance;
			steer += difference;
			count++;
		}
	}

	steer /= (float)count;

	if (Magnitude(steer) > 0.f)
	{
		steer = Normalise(steer);
	}

	return steer;
}

sf::Vector2f Boid::Align(std::vector<Entity>& boids, sf::Vector2f velocity)
{
	float neighbordist = 50.0f;
	sf::Vector2f sum = sf::Vector2f(0.f, 0.f);
	int count = 0;

	for (auto& boid : boids) 
	{
		float distance = Magnitude(mEntity.GetTransform().position - boid.GetTransform().position);
		if ((distance > 0) && (distance < neighbordist))
		{
			sum += velocity;
			count++;
		}
	}

	if (count > 0) {
		sum /= (float)count;
		sum = Normalise(sum);
		sum *= 0.03f;
		sf::Vector2f steer = sum - (Normalise(velocity) * 0.02f);;
		return steer;
	}

	return sf::Vector2f(0.f, 0.f);
}

sf::Vector2f Boid::Cohesion(std::vector<Entity>& boids, sf::Vector2f velocity)
{
	float neighbordist = 50;
	sf::Vector2f sum = sf::Vector2f(0, 0);   
	int count = 0;

	for (auto& boid : boids)
	{
		float distance = Magnitude(mEntity.GetTransform().position - boid.GetTransform().position);

		if ((distance > 0) && (distance < neighbordist)) 
		{
			sum += boid.GetTransform().position;
			count++;
		}
	}
	if (count > 0) 
	{
		sum /= (float)count;
		sf::Vector2f target = sf::Vector2f(0.f, 0.f);
		target = sum - mEntity.GetTransform().position;
		target = Normalise(target);
		sf::Vector2f steer = target - (Normalise(velocity) * 0.02f);
		return steer;
	}
	else
	{
		return  sf::Vector2f(0.f, 0.f);
	}
}

void Boid::ApplyVector(sf::Vector2f vector)
{

}
