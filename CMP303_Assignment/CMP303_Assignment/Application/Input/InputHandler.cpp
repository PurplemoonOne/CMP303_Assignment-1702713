#include "InputHandler.h"
#include "Keyboard.h"

InputHandler::InputHandler()
{
}

void InputHandler::HandleKeyboard(Keyboard * lkeyboard)
{
	for (int index = 0; index < 256; ++index)
	{
		if (lkeyboard->IsKeyPressed(index))
		{
			if (lkeyboard->GetKeyEvent(index) != nullptr)
			{
				quedEvent = lkeyboard->GetKeyEvent(index);
				return;
			}
		}
	}
}
