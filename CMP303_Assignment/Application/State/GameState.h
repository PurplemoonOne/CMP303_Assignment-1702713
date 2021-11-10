#pragma once
#include "State.h"
#include "../GameObject/Entity.h"
#include "../Input/InputHandler.h"
#include <SFML\Network.hpp>

#include <memory>

class InputHandler;

const std::string ipa = "192.168.0.27";


class GameState : public State
{
public:
	GameState();
	virtual ~GameState() override;
private:
	virtual void OnStart() override;
	virtual void OnUpdate(float deltaTime, Keyboard* keyboard = nullptr, Gamepad* gamepad = nullptr) override;
	virtual void OnAttach() override;
	virtual void OnDetach() override;
private:
	InputHandler inputHandler;
	Entity entity;
	float elapsedTime;

private:
	/**	Networking code	**/

	// @brief A socket used to pass data to its peer.
	sf::UdpSocket sendSocket;
	// @brief A socket used to recieve data from its peer.
	sf::UdpSocket recieveSocket;

	int32_t portNumber;

};

