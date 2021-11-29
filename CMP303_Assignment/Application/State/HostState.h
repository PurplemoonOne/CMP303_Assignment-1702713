#pragma once
#include "State.h"
#include "../GameObject/Entity.h"
#include "../Input/InputHandler.h"
#include "../Network/Client.h"

#include <memory>

class Boid
{
public:
	Boid() = default;
	Boid(Scene* scene, std::string tag, sf::Texture* texture);
	Boid(const Boid&) = default;
	~Boid() = default;

	Entity mEntity;
	bool mAlive;
};

class Flock
{
public:
	Flock() = default;
	Flock(const Flock&) = default;
	~Flock() = default;

	void CreateFlock(Scene* scene, sf::Uint32 count);

	std::vector<Boid> mBoids;
	sf::Texture mFishTexture;

};

class HostState : public State
{
public:
	HostState(sf::Vector2f screenDimensions);
	virtual ~HostState() override;

	sf::Uint32 GetBoidCount() const { return mBoidCount; }

	void GenerateClientAssets();
	const bool HasAssets() const { return mHasAssets; }

private:
	virtual void OnStart() override;
	virtual void OnUpdate(float deltaTime, const float appElapsedTime, Keyboard* keyboard = nullptr, Gamepad* gamepad = nullptr) override;
	virtual void OnDetach() override;
private:
	InputHandler inputHandler;
	
	bool QueryButton(Keyboard* keyboard);
	void InitHomeButton();
	void InitUI();
private:
	//Gameplay
	Flock mFlock;
	Entity mShark;

	//UI
	Entity mHomeButton;
	Entity mLives;
	Entity mLivesCount;

	sf::Vector2f mScreenDimensions;
	sf::Uint32 mBoidCount = 0;
	bool mHasAssets = false;
	sf::Texture mSwordFishTexture;
private:
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

