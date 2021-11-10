#pragma once
#include <unordered_map>
#include "../Renderer/Renderer.h"
class State;
class Keyboard;
class Gamepad;
class Scene
{
public:
	Scene(sf::RenderWindow* window);
	~Scene();

	void TransitionState(std::string state);
	void UpdateActiveState(const float time, const float appElapsedTime, Keyboard* keyboard, Gamepad* gamepad, bool debugInfo = false);

	Registery* GetRegistery() { return &registery; }
	void Clean();

private:
	Renderer mRenderer;
	Registery registery;
	State* mActiveState;
	std::unordered_map<std::string, State*> states;

	static Scene* mContext;
};

