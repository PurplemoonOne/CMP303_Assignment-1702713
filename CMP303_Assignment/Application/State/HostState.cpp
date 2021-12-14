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
	//Reserve space.
	mScene->GetRegistery()->ReserveBuffer(80);

	//Create the flock.
	mFlock.CreateFlock(mScene, (mBoidCount = 32));

	//Create graphics.
	InitHomeButton();
	InitBackdrop();

	//Connect to the server.
	mScene->GetClient()->ConnectToServer();
	//Send asset description.
	mScene->GetClient()->SendConnectionInformation(1, mBoidCount, {2.0f, 2.0f});
}

void HostState::OnUpdate(float deltaTime, const float appElapsedTime, Keyboard* keyboard, Gamepad* gamepad)
{
	//Fetch mouse coordinates.
	sf::Vector2f mouseCoordinates = sf::Vector2f(keyboard->MouseX(), keyboard->MouseY());

	AlterBoidParameters(deltaTime, keyboard);

	//Run boid algorithm.
	Seperation(deltaTime);
	//Alignment.
	for (auto& boid : mFlock.mBoids)
	{
		TransformComponent& transform = boid.mEntity.GetTransform();
		sf::Vector2f& boidPos = transform.position;

		//Alignment.
		sf::Vector2f align = mouseCoordinates - boidPos;

		// Calculate rotation.
		float angle = degrees(atan2f(align.x, align.y)) + 180.0f;

		if (Magnitude(align) > 1.0f)
		{
			transform.position += Normalise(align) * 250.0f * deltaTime;

			//flip sprite
			if ((angle > 270.f && angle < 360.f))
			{
				transform.scale = { -2.f, 2.f };
			}
			if ((angle > 360.0f && angle < 90.0f))
			{
				transform.scale = { -2.f, 2.f };
			}
			if ((angle > 90.0f && angle < 180.0f))
			{
				transform.scale = { -2.0f, -2.0f };
			}
			if ((angle > 180.0f) && (angle < 270.0f))
			{
				transform.scale = { 2.f, -2.f };
			}

			transform.rotation = angle;
		}
	}
	Cohesion(deltaTime);


	if (keyboard->IsKeyPressed(sf::Keyboard::Key::Space))
	{
		keyboard->SetKeyUp(sf::Keyboard::Key::Space);
		if (mEnableGhosts)
		{
			mEnableGhosts = false;
		}
		else
		{
			mEnableGhosts = true;
		}
	}

	//Update the position of the sprites.
	for (int i = 0; i < mBoidCount; ++i)
	{
		mScene->GetRegistery()->UpdateSpriteComponent(i);
	}
	

	//Check if we've pressed the home button.
	if (QueryButton(keyboard))
	{
		APP_TRACE("Disconnecting from server....");
		if (mScene->GetClient()->Disconnect())
		{
			mHasGameAssets = false;
			mScene->DeleteClientAndSetNull();

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
		mHomeButton.GetRenderer().sprite.setTexture(mHomeButtonPressTexture);
		mHomeButton.GetRenderer().sprite.setColor(sf::Color(255, 255, 255, 245));
		mHomeButton.GetRenderer().sprite.setScale(0.35f, 0.35f);
		if (keyboard->MouseLeftButtonDown())
		{
			bButtonPressed = true;
		}
	}
	else
	{
		mHomeButton.GetRenderer().sprite.setTexture(mHomeButtonTexture);
		mHomeButton.GetRenderer().sprite.setColor(sf::Color(255, 255, 255, 100));
		mHomeButton.GetRenderer().sprite.setScale(0.25f, 0.25f);
		bButtonPressed = false;
	}

	return bButtonPressed;
}

void HostState::InitHomeButton()
{
	mHomeButtonTexture.loadFromFile("Assets/home.png");
	mHomeButtonPressTexture.loadFromFile("Assets/homeC.png");
	mHomeButton = Entity(mScene, "HomeButton", &mHomeButtonTexture, 66);

	mHomeButton.GetText().font.loadFromFile("Assets/font.ttf");

	//Button text
	mHomeButton.GetText().text.setFont(mHomeButton.GetText().font);
	mHomeButton.GetText().text.setCharacterSize(14);
	mHomeButton.GetText().text.setFillColor(sf::Color::White);
	mHomeButton.GetText().text.setOutlineColor(sf::Color::Black);
	mHomeButton.GetText().text.setOutlineThickness(1.2f);
	mHomeButton.GetText().text.setLetterSpacing(1.5f);
	mHomeButton.GetText().text.setString("Home");

	mHomeButton.GetRenderer().sprite.setPosition(mScreenDimensions * 0.9f);
	mHomeButton.GetRenderer().sprite.setTexture(mHomeButtonTexture);
	mHomeButton.GetRenderer().sprite.setColor(sf::Color(255, 255, 255, 55));
	mHomeButton.GetRenderer().sprite.setScale(0.25f, 0.25f);
	mHomeButton.GetRenderer().sprite.setOrigin(0, 0);
	//Invisible square
	mHomeButton.GetRenderer().graphics.setSize({64.0,64.0});
	mHomeButton.GetRenderer().graphics.setFillColor(sf::Color(255, 255, 255, 0));
	mHomeButton.GetRenderer().graphics.setPosition(mScreenDimensions * 0.9f);
	mHomeButton.GetRenderer().graphics.setOrigin(0, 0);
	mHomeButton.GetText().text.setPosition(mHomeButton.GetRenderer().graphics.getPosition());
}

void HostState::InitBackdrop()
{
	//Load textures
	mBackdropTexture.loadFromFile("Assets/background.png");
	mMiddropTexture.loadFromFile("Assets/midground.png");

	//Mid drop
	mMiddrop = Entity(mScene, "Middrop", &mMiddropTexture, 70);
	mMiddrop.GetRenderer().graphics.setSize({ 0, 0 });
	mMiddrop.GetRenderer().sprite.setPosition(0, 0);
	mMiddrop.GetRenderer().sprite.setScale({ 3,3 });

	//Backdrop
	mBackdrop = Entity(mScene, "Backdrop", &mBackdropTexture, 71);
	mBackdrop.GetRenderer().graphics.setSize({ 0, 0 });
	mBackdrop.GetRenderer().sprite.setPosition(0,0);
	mBackdrop.GetRenderer().sprite.setScale({7,7});

}

void HostState::AlterBoidParameters(const float deltaTime, Keyboard* keyboard)
{
	if (keyboard->IsKeyPressed(sf::Keyboard::Key::Q))
	{
		mCohesionFactor -= 10.0f * deltaTime;
	}
	if (keyboard->IsKeyPressed(sf::Keyboard::Key::E))
	{
		mCohesionFactor += 10.0f * deltaTime;
	}
	if (keyboard->IsKeyPressed(sf::Keyboard::Key::A))
	{
		mSeperationFactor -= 10.0f * deltaTime;
	}
	if (keyboard->IsKeyPressed(sf::Keyboard::Key::D))
	{
		mSeperationFactor += 10.0f * deltaTime;
	}
	if (keyboard->IsKeyPressed(sf::Keyboard::Key::Z))
	{
		mMinDistance -= 50.0f * deltaTime;
	}
	if (keyboard->IsKeyPressed(sf::Keyboard::Key::C))
	{
		mMinDistance += 50.0f * deltaTime;
	}
	if (keyboard->IsKeyPressed(sf::Keyboard::Key::Space))
	{
		mSeperationFactor = 3.7f;
		mCohesionFactor = 0.5f;
		mMinDistance = 75.0f;
	}
	clamp(mSeperationFactor, 0.1f, 200.f);
	clamp(mCohesionFactor, 0.1f, 200.f);
	clamp(mMinDistance, 0.1f, 200.f);
}

void HostState::GenerateClientAssets()
{
	//Will wait until it has a valid pack of assets.
	ConnectionData assets = mScene->GetClient()->RecieveAssetsDescFromClient();

	if (assets.count > 0)
	{
		mSwordFishTexture.loadFromFile("Assets/fishGfx/swordfish.png");
		mShark = Entity(mScene, "shark", &mSwordFishTexture, 65);
		mShark.GetRenderer().bShouldRenderGFX = false;
		mShark.GetRenderer().bShouldRenderSPR = true;
		mShark.GetRenderer().sprite.setScale({ assets.sizeX, assets.sizeX });
		mShark.GetTransform().scale = { assets.sizeX, assets.sizeX };

		mSwordFishTexture.loadFromFile("Assets/fishGfx/swordfish.png");
		mSharkGhost = Entity(mScene, "sharkGhost", &mSwordFishTexture, 68);
		mSharkGhost.GetRenderer().bShouldRenderGFX = false;
		mSharkGhost.GetRenderer().bShouldRenderSPR = true;
		mSharkGhost.GetRenderer().sprite.setScale({ assets.sizeX, assets.sizeX });
		mSharkGhost.GetRenderer().sprite.setColor(sf::Color(125, 125, 125, 125));
		mSharkGhost.GetTransform().scale = { assets.sizeX, assets.sizeX };

		mHasGameAssets = true;
	}
}

inline void HostState::Seperation(const float deltaTime)
{
	sf::Vector2f centroid{};


	for (auto& boid : mFlock.mBoids)
	{
		centroid += boid.mEntity.GetTransform().position;
	}
	centroid *= 0.2f;

	//For each boid...
	for (int i= 0; i < mBoidCount; ++i)
	{
		//For each neighbouring boid...
		for (int j = 0; j < mBoidCount; ++j)
		{
			sf::Vector2f distance{};
			if (mFlock.mBoids.at(j).mEntity.GetTag() != mFlock.mBoids.at(i).mEntity.GetTag())
			{
				sf::Vector2f p0 = mFlock.mBoids.at(j).mEntity.GetTransform().position;
				sf::Vector2f p1 = mFlock.mBoids.at(i).mEntity.GetTransform().position;

				distance = p0 - p1;

				if (Magnitude(distance) < mMinDistance)
				{
					mFlock.mBoids.at(i).mEntity.GetTransform().position += -(distance * mSeperationFactor) * deltaTime;
				}
			}
		}

		mFlock.mBoids.at(i).mEntity.GetTransform().position += -(Normalise(centroid) * mSeperationFactor) * deltaTime;
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
		boid.mEntity.GetTransform().position += Normalise(cohesion) * mCohesionFactor * deltaTime;
	}
}

Boid::Boid(Scene* scene, std::string tag, sf::Texture* texture, sf::Uint32 index)
{
	mEntity = Entity(scene, tag, texture, index);
	float xPos = rand() % 1000 + 1, yPos = rand() % 1000 + 1;
	mEntity.GetRenderer().bShouldRenderGFX = false;
	mEntity.GetRenderer().graphics.setSize({0,0});
	mEntity.GetRenderer().sprite.setOrigin(8.0f, 8.0f);
	mEntity.GetRenderer().sprite.setScale(2.0f, 2.0f);
	mEntity.GetTransform().position = { xPos, yPos };
	mEntity.GetTransform().scale = { 2.0f, 2.0f };
}

void Flock::CreateFlock(Scene* scene, sf::Uint32 count)
{
	//Create fish assets.
	mFishTexture.loadFromFile("Assets/fishGfx/fish.png");
	for (sf::Uint32 i = 0; i < count; ++i)
	{
		mBoids.push_back(Boid(scene, "B" + std::to_string(i), &mFishTexture, i));
	}

}
