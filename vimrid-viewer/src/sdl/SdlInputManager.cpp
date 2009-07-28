#include "SdlInputManager.h"

namespace vimrid
{
namespace sdl
{

SdlInputManager::SdlInputManager()
{
	keyboard = new SdlKeyboardInfo();
	for (int i = 0; i < V_SDL_KEY_MAX; i++)
	{
		keyboard->PressedKeys[i] = false;
	}
}

void SdlInputManager::Update()
{
	this->InputManager::Update();
}

void SdlInputManager::UpdateKeyDown(int sdlKey)
{
	keyboard->PressedKeys[sdlKey] = true;
}

void SdlInputManager::UpdateKeyUp(int sdlKey)
{
	keyboard->PressedKeys[sdlKey] = false;
}

/* Checks if the key has been pressed, and then sets
 * the value of that key to 'not pressed'. This is a
 * quick and dirty fix to solve the issue where X doesn't
 * send us the KeyRelease event for some reason. */
bool SdlInputManager::IsKeyPressed(int sdlKey)
{
	bool pressed = keyboard->PressedKeys[sdlKey];

	/* quick fix: clear the value on query, as the KeyRelease
	 * event type is never given by the X server */
	keyboard->PressedKeys[sdlKey] = false;

	return pressed;
}

}
}
