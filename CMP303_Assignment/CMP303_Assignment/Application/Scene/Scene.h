#pragma once
#include <unordered_map>
#include "../Renderer/Renderer.h"
class State;
class Keyboard;
class Gamepad;
class Scene
{
public:
	Scene();
	~Scene();

	void TransitionState(std::string state);
	void UpdateActiveState(const float time, sf::RenderWindow* window, Keyboard* keyboard, Gamepad* gamepad);

	Registery* GetRegistery() { return &registery; }
	void Clean();

private:
	Renderer renderer;
	Registery registery;
	State* mActiveState;
	std::unordered_map<std::string, State*> states;

	static Scene* mContext;
};

