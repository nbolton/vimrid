/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 30-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef MENU_H_
#define MENU_H_

#include <vimrid.h>
#include "Control.h"
#include <string>

namespace vimrid
{
namespace input
{
class InputManager;
}
namespace ui
{

enum MenuToggleMode
{
	UI_MTM_SHOW = 1,
	UI_MTM_HIDE = 2,
	UI_MTM_SUSPEND = 4,
	UI_MTM_CENTRE_CURSOR = 8
};

class Menu : public Control
{
public:
	Menu();
	virtual ~Menu();
	virtual void Update();
	virtual void Render();
	virtual void OnHidden();
	virtual void OnShown();
	void AddToggleMode(MenuToggleMode toggleMode);
	void RemoveToggleMode(MenuToggleMode toggleMode);
	VBool HasToggleMode(MenuToggleMode toggleMode);
private:
	/*VPoint2f mTrackdCursorStart;
	VPoint2f mTrackdRelativeCursor;*/
	//VPoint3f mCursorStart;
	VBool mMenuButtonState;
	VBool mLastMenuButtonState;
	VBool mVisibleToggled;
	VInt32 mToggleModes;
	void updatePosition();
	VPoint2f getCursorPosition();
	VPoint2f getRelativeMouse();
	void renderMouse();
	void updateTrackdCursor();
	void updateChildCollision();
	void updateVisible();
	void updateMenuToggled();
};

}
}

#endif // MENU_H_
