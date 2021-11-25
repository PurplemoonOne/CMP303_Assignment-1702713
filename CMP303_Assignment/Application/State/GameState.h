#pragma once
#include "State.h"
#include "../GameObject/Entity.h"
#include "../Input/InputHandler.h"
#include "../Network/Client.h"

#include <memory>

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
	Entity mHomeButton;


	sf::Vector2f mScreenDimensions;
	std::vector<Entity> mFlock;
	Entity mShark;
	int mBoidCount = 0;

	// @brief Simple functions to calculate the rules of a boid.
	
	// @note the vector between each boid and the mouse  
	// coordinates will serve as the alignment vector.
	inline void Seperation(const float deltaTime);
	inline void Cohesion(const float deltaTime);


	inline float Magnitude(sf::Vector2f vector)
	{
		return sqrtf((vector.x * vector.x + vector.y * vector.y));
	}

	inline sf::Vector2f Normalise(sf::Vector2f vector)
	{
		return vector / (Magnitude(vector));
	}

};

