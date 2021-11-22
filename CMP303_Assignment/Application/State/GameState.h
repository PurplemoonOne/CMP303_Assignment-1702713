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
	GameState(sf::Vector2f screenDimensions);
	virtual ~GameState() override;
private:
	virtual void OnStart() override;
	virtual void OnUpdate(float deltaTime, const float appElapsedTime, Keyboard* keyboard = nullptr, Gamepad* gamepad = nullptr) override;
	virtual void OnDetach() override;
private:
	InputHandler inputHandler;
	
	bool QueryButton(Keyboard* keyboard);
	void InitHomeButton();
private:
	// @brief An object that represents the playable sprite.
	// @note We only need one instance of entity as we manipulate the correct entity 
	// in memory based on the network ID.
	Entity mEntity;
	Entity mHomeButton;


	sf::Vector2f mScreenDimensions;
	std::vector<Entity> mBoids;
};

struct Boid
{
	Entity mEntity;

	sf::Vector2f Seperate(std::vector<Entity>& boids);
	sf::Vector2f Align(std::vector<Entity>& boids, sf::Vector2f velocity);
	sf::Vector2f Cohesion(std::vector<Entity>& boids, sf::Vector2f velocity);

	void ApplyVector(sf::Vector2f vector);

	inline sf::Vector2f Normalise(sf::Vector2f vector);
	inline float Magnitude(sf::Vector2f vector) { sqrtf(vector.x * vector.x + vector.y * vector.y); }
};