#pragma once
#include <unordered_map>
#include "../Renderer/Renderer.h"
class State;
class Keyboard;
class Gamepad;
class Context
{
public:
	Context();
	~Context();

	void TransitionState(std::string state);
	void UpdateActiveState(const float time, sf::RenderWindow* window, Keyboard* keyboard, Gamepad* gamepad);
private:
	Renderer renderer;

	State* mActiveState;

	std::unordered_map<std::string, State*> states;

	static Context* mContext;
};

