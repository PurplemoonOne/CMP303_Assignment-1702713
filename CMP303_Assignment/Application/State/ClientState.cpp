#include "ClientState.h"
#include "../Input/Keyboard.h"
#include "../Log/Log.h"


ClientState::ClientState(sf::Vector2f screenDimensions)
	:
	mScreenDimensions(screenDimensions),
	mHasAssets(false)
{
}

ClientState::~ClientState()
{
}

void ClientState::OnStart()
{
	//Reserve space.
	mScene->GetRegistery()->ReserveBuffer(80);

	//Connect to the server.
	mScene->GetClient()->ConnectToServer();

	//Send client data to server.
	mScene->GetClient()->SendConnectionInformation(1, 1, {4.0f, 4.0f});

	//Load texture.
	mSwordFishTexture.loadFromFile("Assets/fishGfx/swordfish.png");
	
	//Create shark.
	mShark = Entity(mScene, "shark", &mSwordFishTexture, 65);
	mShark.GetRenderer().sprite.setScale({ 4.0f, 4.0f });
	mShark.GetRenderer().sprite.setOrigin({ 16.0f, 16.0f });
	mShark.GetRenderer().bShouldRenderGFX = false;
	mShark.GetTransform().scale = { 4.0f, 4.0f };

	//Create graphics.
	InitHomeButton();
	InitBackdrop();
}

void ClientState::OnUpdate(float deltaTime, const float appElapsedTime, Keyboard* keyboard, Gamepad* gamepad)
{
	//Fetch mouse coordinates.
	sf::Vector2f mouseCoordinates = sf::Vector2f(keyboard->MouseX(), keyboard->MouseY());

	if ((mouseCoordinates.x > mScreenDimensions.x || mouseCoordinates.x < 0.f) 
		||
		(mouseCoordinates.y > mScreenDimensions.y || mScreenDimensions.y < 0.f))
	{
		mUpdatePosition = false;
	}
	else
	{
		mUpdatePosition = true;
	}

	if (mUpdatePosition)
	{
		sf::Vector2f& sharkPos = mScene->GetRegistery()->GetTransformComponent("shark").position;
		sf::Vector2f& sharkScale = mScene->GetRegistery()->GetTransformComponent("shark").scale;

		sf::Vector2f align = mouseCoordinates - sharkPos;

		//Update shark's position.
		if (Magnitude(align) > 1.0f)
		{
			sharkPos += Normalise(align) * 250.0f * deltaTime;
		}
	
	}


	//Update the position of the sprites.
	mScene->GetRegistery()->UpdateSpriteComponent("shark");

 	if (QueryButton(keyboard))
	{
		APP_TRACE("Disconnecting from server....");
		if (mScene->GetClient()->Disconnect())
		{
			mHasAssets = false;
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

void ClientState::OnDetach()
{
	mScene->GetRegistery()->ClearRegistery();
}

void ClientState::GenerateHostAssets()
{
	//Will wait until it has a valid pack of assets.
	ConnectionData assets = mScene->GetClient()->RecieveAssetsDescFromClient();

	if (assets.count > 0)
	{
		mFishTexture.loadFromFile("Assets/fishGfx/fish.png");
		APP_TRACE("Successfully recieved valid asset descriptions!");

		//Create replicate assets of the host.
		for (sf::Uint32 i = 0; i < assets.count; ++i)
		{
			Entity entity = Entity(mScene, "B" + std::to_string(i), &mFishTexture, i);
			entity.GetRenderer().bShouldRenderGFX = false;
			entity.GetRenderer().bShouldRenderSPR = true;
			entity.GetRenderer().graphics.setSize({ 0,0});
			entity.GetRenderer().sprite.setScale({ assets.sizeX, assets.sizeY });
			entity.GetTransform().scale = { assets.sizeX, assets.sizeY };
		}
		mBoidCount = assets.count;
		mHasAssets = true;
	}
}

void ClientState::InitBackdrop()
{
	//Load textures
	mBackdropTexture.loadFromFile("Assets/background.png");
	mMiddropTexture.loadFromFile("Assets/midground.png");

	//Mid drop
	mMiddrop = Entity(mScene, "Middrop", &mMiddropTexture, 75);
	mMiddrop.GetRenderer().graphics.setSize({ 0, 0 });
	mMiddrop.GetRenderer().sprite.setPosition(0, 0);
	mMiddrop.GetRenderer().sprite.setScale({ 3,3 });

	//Backdrop
	mBackdrop = Entity(mScene, "Backdrop", &mBackdropTexture, 76);
	mBackdrop.GetRenderer().graphics.setSize({ 0, 0 });
	mBackdrop.GetRenderer().sprite.setPosition(0, 0);
	mBackdrop.GetRenderer().sprite.setScale({ 7,7 });

}

bool ClientState::QueryButton(Keyboard* keyboard)
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

void ClientState::InitHomeButton()
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
	mHomeButton.GetRenderer().graphics.setSize({ 64.0,64.0 });
	mHomeButton.GetRenderer().graphics.setFillColor(sf::Color(255, 255, 255, 0));
	mHomeButton.GetRenderer().graphics.setPosition(mScreenDimensions * 0.9f);
	mHomeButton.GetRenderer().graphics.setOrigin(0, 0);
	mHomeButton.GetText().text.setPosition(mHomeButton.GetRenderer().graphics.getPosition());
}
