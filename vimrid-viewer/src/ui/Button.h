/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 02-May-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include "TextControl.h"

namespace vimrid
{
namespace ui
{

enum ButtonSelectAction
{
	UI_BSA_HIDE_PARENT = 1,
	UI_BSA_SHOW_CONTROL = 2,
};

class Button : public TextControl
{
public:
	Button(const std::string &text);
	virtual ~Button();
	virtual void Update();
	virtual void Render();
    virtual void OnSelectPress();
    virtual void OnSelectRelease();
	void AddSelectAction(ButtonSelectAction buttonAction);
	void SetShowControl(Control &menu) { mShowControl = &menu; }
	Control &GetShowControl() { return *mShowControl; }
private:
	VPoint2f mTextPadding;
	VInt32 mButtonAction;
	Control *mShowControl;
};

}
}

#endif // BUTTON_H_
