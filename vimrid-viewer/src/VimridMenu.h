/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 04-May-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef VIMRIDMENU_H_
#define VIMRIDMENU_H_

#include "ui/Menu.h"

namespace vimrid
{
class VimridApplication;

class VimridMenu: public vimrid::ui::Menu
{
public:
	VimridMenu(VimridApplication &application);
	virtual ~VimridMenu();
	virtual void OnShown();
	virtual void OnHidden();
    virtual void OnSelectPress();
    virtual void OnSelectRelease();
	virtual void OnChildSelectPress(Control &child);
	virtual void OnChildSelectRelease(Control &child);
private:
	VimridApplication &mApplication;
};

}

#endif // VIMRIDMENU_H_
