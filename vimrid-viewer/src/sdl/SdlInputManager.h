#ifndef SDLINPUTMANAGER_H_
#define SDLINPUTMANAGER_H_

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include "../input/InputManager.h"

// Taken from the last XK_ definition.
#define V_SDL_KEY_MAX 0x10028ff

namespace vimrid
{
namespace sdl
{

typedef struct
{
	bool PressedKeys[V_SDL_KEY_MAX];
} SdlKeyboardInfo;

class SdlInputManager : public vimrid::input::InputManager
{
public:
	SdlInputManager();
	void UpdateKeyDown(int sdlKey);
	void UpdateKeyUp(int sdlKey);
	bool IsKeyPressed(int sdlKey);
	virtual void Update();
private:
	void removeKey(XKeyEvent* keyEvent);
	SdlKeyboardInfo* keyboard;
};

}
}

#endif // SDLINPUTMANAGER_H_
