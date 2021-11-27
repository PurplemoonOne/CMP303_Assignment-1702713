#include "HostState.h"
#include "../Events/Event.h"
#include "../Input/Keyboard.h"
#include "../Log/Log.h"

HostState::HostState(sf::Vector2f screenDimensions)
	:
	mScreenDimensions(screenDimensions)
{
}

HostState::~HostState()
{	
}

void HostState::OnStart()
{
	//Create the flock.
	mFlock.CreateFlock(mScene, (mBoidCount = 64));

	InitHomeButton();
	InitShark();

	sf::Vector2f size = mFlock.mBoids.back().mEntity.GetRenderer().graphics.getSize();

	mScene->GetClient()->ConnectToServer();

	mScene->GetClient()->SendConnectionInformation(1, mBoidCount, size);
}

void HostState::OnUpdate(float deltaTime, const float appElapsedTime, Keyboard* keyboard, Gamepad* gamepad)
{
	sf::Vector2f mouseCoordinates = sf::Vector2f(keyboard->MouseX(), keyboard->MouseY());
	
	Seperation(deltaTime);
	for (auto& boid : mFlock.mBoids)
	{
		//Alignment.
		sf::Vector2f align = mouseCoordinates - boid.mEntity.GetTransform().position;
		if (Magnitude(align) > 1.0f)
		{
			boid.mEntity.GetTransform().position += Normalise(align) * 250.0f * deltaTime;
		}
	}
	Cohesion(deltaTime);

	//Update the position of the sprites.
	for(int i = 0; i < mBoidCount; ++i)
		mScene->GetRegistery()->UpdateRendererComponent(i);

	if (QueryButton(keyboard))
	{
		APP_TRACE("Disconnecting from server....");
		if (mScene->GetClient()->Disconnect())
		{
			delete mScene->GetClient();
			mScene->TransitionState("menu");
		}
		else
		{
			APP_ERROR("Could not disconnect from the server gracefully.... Terminating program.");
			exit(1);
		}

	}
}

void HostState::OnDetach()
{
	mScene->GetRegistery()->ClearRegistery();
	mBoidCount = 0;
	mFlock.mBoids.clear();
	mFlock.mBoids.resize(0);
}

bool HostState::QueryButton(Keyboard* keyboard)
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

void HostState::InitHomeButton()
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

void HostState::InitUI()
{
	//Lives text.
	mLives = Entity(mScene, "LivesText");
	mLives.GetText().text.setCharacterSize(14);
	mLives.GetText().text.setFillColor(sf::Color::White);
	mLives.GetText().text.setOutlineColor(sf::Color::Black);
	mLives.GetText().text.setOutlineThickness(1.2f);
	mLives.GetText().text.setLetterSpacing(1.5f);
	mLives.GetText().text.setString("Lives : ");

	mLives.GetRenderer().graphics.setFillColor(sf::Color::Black);
	mLives.GetRenderer().graphics.setPosition(128.f, 128.f);
	mLives.GetRenderer().graphics.setSize(sf::Vector2f(256.0f, 32.0f));
	mLives.GetText().text.setPosition(mHomeButton.GetRenderer().graphics.getPosition() + sf::Vector2f(16.f, 16.f));

	//Lives Remaining
	mLivesCount = Entity(mScene, "LivesCount");
	mLivesCount.GetText().text.setCharacterSize(14);
	mLivesCount.GetText().text.setFillColor(sf::Color::White);
	mLivesCount.GetText().text.setOutlineColor(sf::Color::Black);
	mLivesCount.GetText().text.setOutlineThickness(1.2f);
	mLivesCount.GetText().text.setLetterSpacing(1.5f);
	mLivesCount.GetText().text.setString("");

	mLivesCount.GetRenderer().graphics.setFillColor(sf::Color::Black);
	mLivesCount.GetRenderer().graphics.setPosition(132.f, 128.f);
	mLivesCount.GetRenderer().graphics.setSize(sf::Vector2f(256.0f, 32.0f));
	mLivesCount.GetText().text.setPosition(mHomeButton.GetRenderer().graphics.getPosition() + sf::Vector2f(16.f, 16.f));
}

void HostState::InitShark()
{
	mShark = Entity(mScene, "shark");
	mShark.GetRenderer().graphics.setFillColor(sf::Color::Red);
	mShark.GetRenderer().graphics.setSize(sf::Vector2f(128.0f, 128.0f));
	mShark.GetTransform().position = sf::Vector2f(rand() % 1000 + 1, rand() % 1000 + 1);
}

inline void HostState::Seperation(const float deltaTime)
{
	sf::Vector2f centroid{};
	float minDistance = 75.0f;
	float seperationFactor = 2.7;

	for (auto& boid : mFlock.mBoids)
	{
		centroid += boid.mEntity.GetTransform().position;
	}
	centroid *= 0.2f;


	for (int i= 0; i < mBoidCount; ++i)
	{
		for (int j = 0; j < mBoidCount; ++j)
		{
			sf::Vector2f distance{};
			if (mFlock.mBoids.at(j).mEntity.GetTag() != mFlock.mBoids.at(i).mEntity.GetTag())
			{
				sf::Vector2f p0 = mFlock.mBoids.at(j).mEntity.GetTransform().position;
				sf::Vector2f p1 = mFlock.mBoids.at(i).mEntity.GetTransform().position;

				distance = p0 - p1;

				if (Magnitude(distance) < minDistance)
				{
					mFlock.mBoids.at(i).mEntity.GetTransform().position += -(distance * seperationFactor) * deltaTime;
				}
			}
		}

		mFlock.mBoids.at(i).mEntity.GetTransform().position += -(Normalise(centroid) * seperationFactor) * deltaTime;
	}
}

inline void HostState::Cohesion(const float deltaTime)
{
	sf::Vector2f centroid{};

	for (auto& boid : mFlock.mBoids)
	{
		centroid += boid.mEntity.GetTransform().position;
	}
	centroid *= 0.2f;

	sf::Vector2f cohesion{};

	for (auto& boid : mFlock.mBoids)
	{
		cohesion = centroid - boid.mEntity.GetTransform().position;
		boid.mEntity.GetTransform().position += Normalise(cohesion) * 0.5f * deltaTime;
	}
}

Boid::Boid(Scene* scene, std::string tag)
{
	mEntity = Entity(scene, tag);
	mEntity.GetRenderer().graphics.setSize(sf::Vector2f(32.0f, 32.0f));
	mEntity.GetRenderer().graphics.setFillColor(sf::Color::Green);
	float xPos = rand() % 1000 + 1, yPos = rand() % 1000 + 1;
	mEntity.GetRenderer().graphics.setPosition(xPos, yPos);
	mEntity.GetTransform().position = sf::Vector2f(xPos, yPos);
}

void Flock::CreateFlock(Scene* scene, sf::Uint32 count)
{
	for (sf::Uint32 i = 0; i < count; ++i)
	{
		mBoids.push_back(Boid(scene, "B" + std::to_string(i)));
	}
}
