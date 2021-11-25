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
	const sf::Color colours[] = { sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::White, sf::Color::Yellow, sf::Color::Magenta };

	int c = 0;
	for (int i = 0; i < 24; ++i)
	{
		//Create six boid objects.
		mFlock.push_back(Entity(mScene, "B" + std::to_string(i)));
		mFlock.back().GetRenderer().graphics.setSize(sf::Vector2f(32.0f, 32.0f));
		mFlock.back().GetRenderer().graphics.setFillColor(colours[c]);
		float xPos = rand() % 1000 + 1, yPos = rand() % 1000 + 1;
		mFlock.back().GetRenderer().graphics.setPosition(xPos, yPos);
		mFlock.back().GetTransform().position = sf::Vector2f(xPos, yPos);

		i == 5 || i == 11 || i == 17 ? c = 0 : ++c;
		mBoidCount++;
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
	
	Seperation(deltaTime);
	for (auto& boid : mFlock)
	{
		//Alignment.
		sf::Vector2f align = mouseCoordinates - boid.GetTransform().position;
		if (Magnitude(align) > 0.2f)
		{
			boid.GetTransform().position += Normalise(align) * 250.0f * deltaTime;
		}
	}
	Cohesion(deltaTime);

	//Update the position of the sprites.
	for(int i = 0; i < mBoidCount; ++i)
		mScene->GetRegistery()->UpdateRendererComponent(i);


	if (QueryButton(keyboard))
	{
		mScene->GetClient()->Disconnect();
		mScene->TransitionState("menu");
	}
}

void GameState::OnDetach()
{
	mScene->GetRegistery()->ClearRegistery();
	mBoidCount = 0;
	mFlock.clear();
	mFlock.resize(0);
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

	mHomeButton.GetText().font.loadFromFile("Assets/font.ttf");

	//Button text
	mHomeButton.GetText().text.setFont(mHomeButton.GetText().font);
	mHomeButton.GetText().text.setCharacterSize(14);
	mHomeButton.GetText().text.setFillColor(sf::Color::White);
	mHomeButton.GetText().text.setOutlineColor(sf::Color::Black);
	mHomeButton.GetText().text.setOutlineThickness(1.2f);
	mHomeButton.GetText().text.setLetterSpacing(1.5f);
	mHomeButton.GetText().text.setString("Home");

	mHomeButton.GetRenderer().graphics.setFillColor(sf::Color::Black);
	mHomeButton.GetRenderer().graphics.setPosition(mScreenDimensions * 0.8f);
	mHomeButton.GetRenderer().graphics.setSize(sf::Vector2f(128.0f, 128.0f));
	mHomeButton.GetRenderer().graphics.setOrigin(0, 0);
	mHomeButton.GetText().text.setPosition(mHomeButton.GetRenderer().graphics.getPosition());
}

inline void GameState::Seperation(const float deltaTime)
{
	sf::Vector2f centroid{};
	float minDistance = 50.0f;
	float seperationFactor = 2.7;

	for (auto& boid : mFlock)
	{
		centroid += boid.GetTransform().position;
	}
	centroid *= 0.2f;


	for (int i= 0; i < 24; ++i)
	{
		for (int j = 0; j < 24; ++j)
		{
			sf::Vector2f distance{};
			if (mFlock.at(j).GetTag() != mFlock.at(i).GetTag())
			{
				sf::Vector2f p0 = mFlock.at(j).GetTransform().position;
				sf::Vector2f p1 = mFlock.at(i).GetTransform().position;

				distance = p0 - p1;

				if (Magnitude(distance) < minDistance)
				{
					mFlock.at(i).GetTransform().position += -(distance * seperationFactor) * deltaTime;
				}
			}
		}

		mFlock.at(i).GetTransform().position += -(Normalise(centroid) * seperationFactor) * deltaTime;
	}
}

inline void GameState::Cohesion(const float deltaTime)
{
	sf::Vector2f centroid{};

	for (auto& boid : mFlock)
	{
		centroid += boid.GetTransform().position;
	}
	centroid *= 0.2f;

	sf::Vector2f cohesion{};

	for (auto& boid : mFlock)
	{
		cohesion = centroid - boid.GetTransform().position;
		boid.GetTransform().position += Normalise(cohesion) * 0.5f * deltaTime;
	}
}

