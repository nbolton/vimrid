#ifndef GLUTINPUTMANAGER_H_
#define GLUTINPUTMANAGER_H_

#include <GL/glut.h>
#include "../input/InputManager.h"
#include "GlutKeyboardInfo.h"
#include <vector>

// Backspace, enter, escape and delete are ASCII chars.
#define GLUT_KEY_BACKSPACE 8
#define GLUT_KEY_ENTER 13
#define GLUT_KEY_ESCAPE 27
#define GLUT_KEY_DELETE 127

namespace vimrid
{
namespace glut
{

typedef unsigned int SpecialKey;
typedef unsigned char CharKey;

class GlutInputManager : public vimrid::input::InputManager
{
public:
	GlutInputManager();
	virtual void Update();
	void UpdateMouseButton(int glutButton, int glutState);
	void UpdateCharKeyDown(CharKey charKey);
	void UpdateCharKeyUp(CharKey charKey);
	void UpdateSpecialKeyDown(SpecialKey specialKey);
	void UpdateSpecialKeyUp(SpecialKey specialKey);
	bool IsCharKeyPressed(CharKey charKey) const;
	bool IsSpecialKeyPressed(SpecialKey specialKey) const;
	bool IsModifierKeyPressed(int modifierKey) const;
	void UpdateKeyModifiers(int modifierKeys);
private:
	GlutKeyboardInfo mKeyboard;
};

}
}

#endif // GLUTINPUTMANAGER_H_
