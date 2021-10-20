#pragma once

class Gamepad;
class Keyboard;
class Scene;
struct Registery;

namespace sf
{
	class RenderWindow;
}

class State
{
public:
	virtual ~State() = default;
	virtual void OnStart()	= 0;
	virtual void OnUpdate(float deltaTime, Keyboard* keyboard = nullptr, Gamepad* gamepad = nullptr) = 0;
	virtual void OnAttach() = 0;
	virtual void OnDetach() = 0;

	void SetScene(Scene* lscene)
	{
		this->mScene = lscene;
	}


protected:
	Scene* mScene;
};
