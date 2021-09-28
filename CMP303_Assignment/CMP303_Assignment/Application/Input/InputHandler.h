#pragma once

class Keyboard;
class Gamepad;
class Event;

class InputHandler
{
public:
	InputHandler();
	virtual ~InputHandler() = default;

	void HandleKeyboard(Keyboard* keyboard);
	Event* QuedEvent() const { return quedEvent; }
private:
	Event* quedEvent;
};


