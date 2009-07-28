/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 22-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef MOUSEINFO_H_
#define MOUSEINFO_H_

#include <vimrid.h>

namespace vimrid
{
namespace input
{

enum MouseButton
{
	MOUSE_BUTTON_LEFT = 1,
	MOUSE_BUTTON_RIGHT = 2,
};

class MouseInfo
{
public:
	MouseInfo();
	virtual ~MouseInfo();
	const VPoint2f &GetPosition() const { return mPosition; }
	void UpdateX(const VUInt32 &x) { mPosition.X = x; }
	void UpdateY(const VUInt32 &y) { mPosition.Y = y; }
	VBool IsLeftButtonPressed() const { return mLeftButtonPressed; }
	VBool IsRightButtonPressed() const { return mRightButtonPressed; }
	VBool IsButtonPressed(MouseButton mouseButton) const;
	VBool WasButtonToggled(MouseButton mouseButton) const;
	void UpdateButtons(VBool left, VBool right);
	void Update();
private:
	VPoint2f mPosition;
	VBool mLeftButtonPressed;
	VBool mRightButtonPressed;
	VBool mLeftButtonPressedLast;
	VBool mRightButtonPressedLast;
	VBool mLeftButtonToggled;
	VBool mRightButtonToggled;
};

}
}

#endif // MOUSEINFO_H_
