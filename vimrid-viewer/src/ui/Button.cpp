/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 02-May-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "Button.h"
#include "Menu.h"

using namespace std;

namespace vimrid
{
namespace ui
{

Button::Button(const string &text)
:
	TextControl(text),
	mTextPadding(8, 11.5),
	mShowControl(NULL)
{
}

Button::~Button()
{
}

void Button::AddSelectAction(ButtonSelectAction buttonAction)
{
	mButtonAction |= (VUInt32)buttonAction;
}

void Button::Update()
{
	this->TextControl::Update();
}

void Button::Render()
{
	TranslateToPosition();
	ChangeColour(GetBackground());
	DrawRectangle(VPoint2f(0, 0), GetSize());

	VSizeF highlightSize = GetSize();
	highlightSize.Width += 2;
	highlightSize.Height += 2;
	DrawRectangleOutline(VPoint2f(-1, -1), highlightSize, true, IsSelected());

	TranslateToPosition(mTextPadding);
	this->TextControl::Render();
}

void Button::OnSelectPress()
{
	DEBUG("Button pressed: " << GetName());

	this->TextControl::OnSelectPress();
}

void Button::OnSelectRelease()
{
	DEBUG("Button released: " << GetName());

	if ((mButtonAction & UI_BSA_SHOW_CONTROL) != 0)
	{
		GetShowControl().Show();
	}

	if ((mButtonAction & UI_BSA_HIDE_PARENT) != 0)
	{
		GetParent().Hide();
	}

	this->TextControl::OnSelectRelease();
}

}
}
