#pragma once
#include <cstdint>
class Gamepad;
class Keyboard;
class Scene;

#include <string>

class State
{
public:
	virtual ~State() = default;
	virtual void OnStart()	= 0;
	virtual void OnUpdate(float deltaTime, const float appElapsedTime, Keyboard* keyboard = nullptr, Gamepad* gamepad = nullptr) = 0;
	virtual void OnDetach() = 0;

	void SetScene(Scene* lscene)
	{
		this->mScene = lscene;
	}

	void SetShouldUpdate(bool value)
	{
		mUpdate = value;
	}

	const bool Update() const { return mUpdate; }

	void SetHasAssets(bool value)
	{
		mHasGameAssets = value;
	}

	const bool HasGameAssets() { return mHasGameAssets; }
	bool IsGhostsEnabled() const { return mEnableGhosts; }

	void SetIpAddress(const std::string& string) { mIpAddress = string; }
	const std::string& GetIpAddress() const { return mIpAddress; }

protected:
	Scene* mScene;
	bool mUpdate = true;
	bool mHasGameAssets = false;
	bool mEnableGhosts = false;
	std::string mIpAddress = " ";
};
