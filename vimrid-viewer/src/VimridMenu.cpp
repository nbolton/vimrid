/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 04-May-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "VimridMenu.h"
#include "VimridApplication.h"

namespace vimrid
{

VimridMenu::VimridMenu(VimridApplication &application)
:
	mApplication(application)
{
}

VimridMenu::~VimridMenu()
{
}

void VimridMenu::OnShown()
{
	this->Menu::OnShown();
	mApplication.OnControlShown(*this);
}

void VimridMenu::OnHidden()
{
	this->Menu::OnHidden();
	mApplication.OnControlHidden(*this);
}

void VimridMenu::OnSelectPress()
{
	this->Menu::OnSelectPress();
	mApplication.OnControlSelectPress(*this);
}

void VimridMenu::OnSelectRelease()
{
	this->Menu::OnSelectRelease();
	mApplication.OnControlSelectRelease(*this);
}

void VimridMenu::OnChildSelectPress(Control &child)
{
	this->Menu::OnChildSelectPress(child);
	mApplication.OnControlSelectPress(child);
}

void VimridMenu::OnChildSelectRelease(Control &child)
{
	this->Menu::OnChildSelectRelease(child);
	mApplication.OnControlSelectRelease(child);
}

}
