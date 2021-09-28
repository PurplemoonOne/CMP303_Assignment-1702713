#pragma once

class Gamepad;
class Keyboard;
class Context;

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
	//virtual void OnRender(sf::RenderWindow* window) = 0;
	virtual void OnAttach() = 0;
	virtual void OnDetach() = 0;

	void SetContext(Context* lcontext)
	{
		this->mContext = lcontext;
	}


protected:
	Context* mContext;
};
