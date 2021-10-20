#include "InputHandler.h"
#include "Keyboard.h"

InputHandler::InputHandler()

{
}

Event* InputHandler::HandleKeyboard(Keyboard * lkeyboard)
{
	lkeyboard->MapEvent(sf::Keyboard::Space, &jump);

	for (int index = 0; index < 256; ++index)
	{
		if (lkeyboard->IsKeyPressed(index))
		{
			if (lkeyboard->GetKeyEvent(index) != nullptr)
			{
				return lkeyboard->GetKeyEvent(index);
			}
		}
	}

	return nullptr;
}
