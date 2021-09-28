#pragma once
#include <unordered_map>
#include <SFML/Graphics.hpp>
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
	State* mActiveState;

	std::unordered_map<std::string, State*> states;

	static Context* mContext;
};

