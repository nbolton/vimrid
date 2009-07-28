/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 22-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "MouseInfo.h"

namespace vimrid
{
namespace input
{

MouseInfo::MouseInfo() :
	mLeftButtonPressed(false),
	mRightButtonPressed(false),
	mLeftButtonPressedLast(false),
	mRightButtonPressedLast(false),
	mLeftButtonToggled(false),
	mRightButtonToggled(false)
{
}

MouseInfo::~MouseInfo()
{
}

void MouseInfo::Update()
{
	// Compare the last state with current state to indicate toggles.
	mLeftButtonToggled = mLeftButtonPressed != mLeftButtonPressedLast;
	mRightButtonToggled = mRightButtonPressed != mRightButtonPressedLast;

	// Store the press states for comparison next time.
	mLeftButtonPressedLast = mLeftButtonPressed;
	mRightButtonPressedLast = mRightButtonPressed;
}

void MouseInfo::UpdateButtons(VBool left, VBool right)
{
	mLeftButtonPressed = left;
	mRightButtonPressed = right;
}

VBool MouseInfo::IsButtonPressed(MouseButton mouseButton) const
{
	switch (mouseButton)
	{
	case MOUSE_BUTTON_LEFT: return mLeftButtonPressed; break;
	case MOUSE_BUTTON_RIGHT: return mRightButtonPressed; break;
	}
	return false;
}

VBool MouseInfo::WasButtonToggled(MouseButton mouseButton) const
{
	switch (mouseButton)
	{
	case MOUSE_BUTTON_LEFT: return mLeftButtonToggled; break;
	case MOUSE_BUTTON_RIGHT: return mRightButtonToggled; break;
	}
	return false;
}

}
}
