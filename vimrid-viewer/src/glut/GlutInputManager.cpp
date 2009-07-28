#include "GlutInputManager.h"

namespace vimrid
{
namespace glut
{

GlutInputManager::GlutInputManager() :
	mKeyboard()
{
	for (int i = 0; i < V_GLUT_SPECIAL_KEY_MAX; i++)
	{
		mKeyboard.SpecialPressed[i] = false;
	}

	for (int i = 0; i < V_GLUT_CHAR_KEY_MAX; i++)
	{
		mKeyboard.CharPressed[i] = false;
	}
}

void GlutInputManager::Update()
{
	this->InputManager::Update();
}

void GlutInputManager::UpdateCharKeyDown(CharKey charKey)
{
	mKeyboard.CharPressed[charKey] = true;
}

void GlutInputManager::UpdateCharKeyUp(CharKey charKey)
{
	mKeyboard.CharPressed[charKey] = false;
}

void GlutInputManager::UpdateSpecialKeyDown(SpecialKey specialKey)
{
	mKeyboard.SpecialPressed[specialKey] = true;
}

void GlutInputManager::UpdateSpecialKeyUp(SpecialKey specialKey)
{
	mKeyboard.SpecialPressed[specialKey] = false;
}

bool GlutInputManager::IsCharKeyPressed(CharKey charKey) const
{
	return mKeyboard.CharPressed[charKey];
}

bool GlutInputManager::IsSpecialKeyPressed(SpecialKey specialKey) const
{
	return mKeyboard.SpecialPressed[specialKey];
}

void GlutInputManager::UpdateMouseButton(int glutButton, int glutState)
{
	UpdateMouseButtons(
		(glutButton == GLUT_LEFT_BUTTON) && (glutState == GLUT_DOWN),
		(glutButton == GLUT_RIGHT_BUTTON) && (glutState == GLUT_DOWN));
}

void GlutInputManager::UpdateKeyModifiers(int modifierKeys)
{
	mKeyboard.ModifierKeys = modifierKeys;
}

bool GlutInputManager::IsModifierKeyPressed(int modifierKey) const
{
	return ((mKeyboard.ModifierKeys & modifierKey) != 0);
}

}
}
