#pragma once
#include "State.h"
#include "../GameObject/Entity.h"
#include "../Input/InputHandler.h"
#include "../Network/Client.h"

#include <memory>

class InputHandler;

class GameState : public State
{
public:
	GameState();
	virtual ~GameState() override;

	Client* GetClient() const { return mClient; }
	const sf::Vector2f& GetClientPosition() { return mEntity.GetTransform(mEntity.GetTag()).position; }


private:
	virtual void OnStart() override;
	virtual void OnUpdate(float deltaTime, const float appElapsedTime, Keyboard* keyboard = nullptr, Gamepad* gamepad = nullptr) override;
	virtual void OnAttach() override;
	virtual void OnDetach() override;
private:
	InputHandler inputHandler;
	Entity mEntity;
	float networkTickRate;

private:
	/**	Networking code	**/
	Client* mClient;
};

