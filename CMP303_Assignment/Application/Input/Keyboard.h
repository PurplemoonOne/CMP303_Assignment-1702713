#pragma once
#include <SFML/Window/Keyboard.hpp>

class Event;

class Keyboard
{
public:
	Keyboard()
	{
		mMouse.x = 0;
		mMouse.y = 0;
		mMouse.left = false;
		mMouse.right = false;
	}

	virtual ~Keyboard()
	{

	}


private:
	// @brief A basic structure representing the data for the mouse.
	struct Mouse
	{
		int x, y;
		bool left, right;
	};

	// @brief A struct representing a 'key' object.
	struct Keys
	{
		bool isDown;
		Event* event;
	};
public:
	// @brief Force a key from the keyboard to be 'down'.
	// @param[in] The numerical 'value' representing a key.
	void SetKeyDown(int code)
	{
		if (code > 0)
			keys[code].isDown = true;
	}

	// @brief Force a key from the keyboard to be 'up'.
	// @param[in] The numerical 'value' representing a key.
	void SetKeyUp(int code)
	{
		if (code > 0)
			keys[code].isDown = false;
	}

	// @brief Query the state of a given key.
	// @param[in] The numerical 'value' representing a key.
	bool IsKeyPressed(int code) const
	{
		if (code > 0)
			return keys[code].isDown;
		else
			return false;
	}

	// @brief Set the 'X' position of the cursor.
	// @param[in] Recieves a signed integer value representing the 'X' position.
	inline void SetMousePosX(int lx) { mMouse.x = lx; }

	// @brief Set the 'Y' position of the cursor.
	// @param[in] Recieves a integer value representing the 'Y' position.
	inline void SetMousePosY(int ly) { mMouse.y = ly; }

	// @brief Set the 'Y' position of the cursor.
	// @param[in] Recieves a integer value representing the 'X' position.
	// @param[in] Recieves a integer value representing the 'Y' position.
	inline void SetMousePos(int lx, int ly) { mMouse.x = lx; mMouse.y = ly; }

	// @brief Returns the 'X' position of the mouse. 
	const int MouseX() const { return mMouse.x; }

	// @brief Returns the 'Y' position of the mouse. 
	const int MouseY() const { return mMouse.y; }

	// @brief Force the left mouse button to be 'down'.
	inline void SetMouseLeftButton(bool down) { mMouse.left = down; }

	// @brief Force the right mouse button to be 'down'.
	inline void SetMouseRightButton(bool down) { mMouse.right = down; }

	// @brief Return the state of the left mouse button.
	const bool MouseLeftButtonDown() const { return mMouse.left; }

	// @brief Return the state of the right mouse button.
	const bool MouseRightButtonDown() const { return mMouse.right; }

	// @brief Return a specified key object.
	// @param[in] The element in the keys array to be accessed.
	Event* GetKeyEvent(int code) const
	{
		if (code > 0)
		{
			return keys[code].event;
		}
		return nullptr;
	}

	// @brief Map an event to a key object.
	// @param[in] The element in the keys array to be accessed.
	// @param[in] A pointer to the event object.
	void MapEvent(int code, Event* event)
	{
		if (code > 0)
		{
			if (keys[code].event == nullptr)
			{
				keys[code].event = event;
			}
		}
	}

private:
	Keys keys[256]{ false, nullptr };
	Mouse mMouse;
};

