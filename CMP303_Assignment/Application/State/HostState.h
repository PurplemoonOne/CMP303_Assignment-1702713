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
	Boid(Scene* scene, std::string tag, sf::Texture* texture, sf::Uint32 index);
	Boid(const Boid&) = default;
	~Boid() = default;

	Entity mEntity;

	// @brief The world's right axis.
	sf::Vector2f rightWorld = { 1.0f, 0.0f };

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

private:
	virtual void OnStart() override;
	virtual void OnUpdate(float deltaTime, const float appElapsedTime, Keyboard* keyboard = nullptr, Gamepad* gamepad = nullptr) override;
	virtual void OnDetach() override;
private:
	InputHandler inputHandler;
	
	bool QueryButton(Keyboard* keyboard);
	void InitHomeButton();
	void InitBackdrop();

	void AlterBoidParameters(const float deltaTime, Keyboard* keyboard);
	float mSeperationFactor = 3.7f;
	float mCohesionFactor = 0.5f;
	float mMinDistance = 75.f;

private:
	//Gameplay
	Flock mFlock;
	Entity mShark;
	sf::Texture mSwordFishTexture;

	//UI
	Entity mHomeButton;
	sf::Texture mHomeButtonTexture;
	sf::Texture mHomeButtonPressTexture;

	//Background
	Entity mBackdrop;
	Entity mMiddrop;
	sf::Texture mBackdropTexture;
	sf::Texture mMiddropTexture;

	//Misc
	sf::Vector2f mScreenDimensions;
	sf::Uint32 mBoidCount = 0;


private:
	// @brief Simple functions to calculate the rules of a boid.
	
	// @note the vector between each boid and the mouse  
	// coordinates will serve as the alignment vector.
	inline void Seperation(const float deltaTime);
	inline void Cohesion(const float deltaTime);

//Utility functions

	inline float Magnitude(sf::Vector2f vector)
	{
		return sqrtf((vector.x * vector.x + vector.y * vector.y));
	}

	inline sf::Vector2f Normalise(sf::Vector2f vector)
	{
		return vector / (Magnitude(vector));
	}

#define min(a, mi) if(a < mi) a = mi
#define max(a, mx) if(a > mx) a = mx
#define clamp(t, a, b) min(t, a); max(t, b)

#define pi 3.14159265359f
#define radians(a) (a * pi) / 180.f
#define degrees(a) (a * 180.0f) / pi

	inline sf::Vector2f RotateVector(sf::Vector2f p0, const float alpha)
	{
		return
		{
			cosf(radians(alpha)) * p0.x - sinf(radians(alpha)) * p0.y,
			sinf(radians(alpha)) * p0.x + cosf(radians(alpha)) * p0.y
		};
	}

	inline const float Angle(const sf::Vector2f v1, const sf::Vector2f v2)
	{
		return acosf(radians(((v1.x * v2.x) + (v1.y * v2.y)) / sqrtf((v1.x * v1.x) + (v1.y * v1.y)) * sqrtf((v2.x * v2.x) + (v2.y * v2.y))));
	}
};

