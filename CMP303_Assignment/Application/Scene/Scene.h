#pragma once
#include <unordered_map>
#include "../Renderer/Renderer.h"
#include "../Network/Client.h"

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

	void HostNetworking(const float deltaTime, const float appElapsedTime);
	void ClientNetworking(const float deltaTime, const float appElapsedTime);

	// @brief Prediction functions to smooth out desparities between client and server.
	inline sf::Vector2f LinearPrediction(const GameData& messageA, const GameData& messageB, int index);
	inline sf::Vector2f QuadraticPrediction(const GameData& messageA, const GameData& messageB, const GameData& messageC, int index);
	const float Lerp(float a, float b, float t);

	// @brief Initialises the latency graphic.
	void InitLatencyGraphic();
	bool mInitLatencyGraphic;

	// @brief track how long it's been since last tick.
	double mTimeElapsedSinceLastUpdate = 0.0;
	// @brief Current system time.
	double mCurrentTimeOnUpdateNetwork = 0.0;
	// @brief Last system time on update.
	double mLastTimeOnUpdateNetwork = 0.0;

private:
	sf::Font mLatencyFont;
	Renderer mRenderer;
	Registery mRegistery;
	State* mActiveState;
	std::unordered_map<std::string, State*> mStates;
	static Scene* mContext;

	float mLerp = 0.f;
	float mLerpSpeed = 100.0f;
	float mLerpThreshold = 0.1f;
	bool mLerpComplete = true;

	sf::Vector2f mWindowMaxBoundaries;
};

