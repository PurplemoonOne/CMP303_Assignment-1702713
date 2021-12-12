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

private:
	void InitBackdrop();
	bool QueryButton(Keyboard* keyboard);
	void InitHomeButton();

	//Assets.
	sf::Texture mFishTexture;
	Entity mShark;
	sf::Texture mSwordFishTexture;

	//Background
	Entity mBackdrop;
	Entity mMiddrop;
	sf::Texture mBackdropTexture;
	sf::Texture mMiddropTexture;

private:
	InputHandler inputHandler;

	sf::Vector2f mScreenDimensions;
	sf::Uint32 mBoidCount = 0;
	Entity mHomeButton;
	sf::Texture mHomeButtonTexture;
	sf::Texture mHomeButtonPressTexture;

	bool mUpdatePosition = true;
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