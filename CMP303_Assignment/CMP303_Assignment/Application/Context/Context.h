#pragma once
#include <unordered_map>

class State;
class Context
{
public:
	Context();
	~Context();

	void TransitionState(std::string state);
	void UpdateActiveState(float time);
private:
	State* mActiveState;

	std::unordered_map<std::string, State*> states;

	static Context* mContext;
};

