#pragma once
#include "../Events/Jump.h"

class Keyboard;
class Gamepad;
class Event;

class InputHandler
{
public:
	InputHandler();
	virtual ~InputHandler() = default;

	Event* HandleKeyboard(Keyboard* keyboard);
private:
	Move move;
};


