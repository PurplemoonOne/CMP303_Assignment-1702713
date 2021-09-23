#pragma once

class Context;
class State
{
public:
	virtual ~State() = default;
	virtual void OnStart()	= 0;
	virtual void OnUpdate(float deltaTime) = 0;
	virtual void OnRender() = 0;
	virtual void OnAttach() = 0;
	virtual void OnDetach() = 0;

	void SetContext(Context* lcontext)
	{
		this->mContext = lcontext;
	}

protected:
	Context* mContext;
};
