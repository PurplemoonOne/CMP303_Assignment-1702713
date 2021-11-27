#pragma once
#include <unordered_map>
#include "../Renderer/Renderer.h"
#include "../Network/Client.h"

class State;
class Keyboard;
class Gamepad;

#define lerp(a,b,t) a * (1 - t) + b * t

class Scene
{
public:
	Scene(sf::RenderWindow* window);
	~Scene();

	void TransitionState(std::string state);
	void UpdateActiveState(const float time, const float appElapsedTime, Keyboard* keyboard, Gamepad* gamepad, bool debugInfo = false);

	Registery* GetRegistery() { return &mRegistery; }
	void Clean();

	// @brief Create a new client object.
	void CreateClient(ClientPrivelage privelage);
	Client* GetClient() { return mClient; }

private:
	/**	Networking code	**/
	Client* mClient;

	float mNetworkTickRate;
	float mTickUpdateThreshold;

	void HostNetworking(const float appElapsedTime);
	void ClientNetworking(const float appElapsedTime);

	// @brief Prediction functions to smooth out desparities between client and server.
	inline sf::Vector2f LinearPrediction(const GameData& messageA, const GameData& messageB, int index);
	inline sf::Vector2f QuadraticPrediction(const GameData& messageA, const GameData& messageB, const GameData& messageC, int index);

private:
	Renderer mRenderer;
	Registery mRegistery;
	State* mActiveState;
	std::unordered_map<std::string, State*> mStates;
	static Scene* mContext;
};

