#pragma once
#include "State.h"
#include "../GameObject/Entity.h"
#include "../Input/InputHandler.h"
#include "../Network/Client.h"

class ClientState : public State
{
public:
	ClientState(sf::Vector2f screenDimensions);
	virtual ~ClientState();

	sf::Uint32 GetBoidCount() const { return mBoidCount; }

	virtual void OnStart() override;
	virtual void OnUpdate(float deltaTime, const float appElapsedTime, Keyboard* keyboard = nullptr, Gamepad* gamepad = nullptr) override;
	virtual void OnDetach() override;

	void GenerateHostAssets();
	const bool HasAssets() const { return mHasAssets; }
private:
	bool QueryButton(Keyboard* keyboard);
	void InitHomeButton();

private:
	InputHandler inputHandler;
	Entity mShark;
	sf::Vector2f mScreenDimensions;
	sf::Uint32 mBoidCount = 0;
	Entity mHomeButton;
	bool mHasAssets = false;
};