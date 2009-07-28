/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 05-May-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "Container.h"
#include <vector>
#include "Menu.h"

using namespace std;

namespace vimrid
{
namespace ui
{

Container::Container(VInt32 renderIndex)
:
	Control(renderIndex)
{
	Show();
}

Container::~Container()
{
}

void Container::Update()
{
	UpdateChildren();
}

void Container::Render()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Use orthographic projection for 2D where 0,0 is top left.
	const VSizeF &windowSize = GetUtility().GetWindowSize();
	glOrtho(0, windowSize.Width, windowSize.Height, 0, -1, 1);

	TranslateToPosition();
	RenderChildren();
}

VBool Container::IsMenuToggleSuspended()
{
	VBool foundSuspend = false;
	vector<Control*> children = GetChildren();
	for (size_t i = 0; i < children.size(); i++)
	{
		// Attempt to convert to menu (may be of other type).
		Menu *menuPtr = dynamic_cast<Menu*>(children[i]);

		// If type is menu, visible and has suspend mode, then suspend must occur.
		if ((menuPtr != NULL)
			&& menuPtr->IsVisible()
			&& menuPtr->HasToggleMode(UI_MTM_SUSPEND))
		{
			foundSuspend = true;
			break;
		}
	}
	return foundSuspend;
}


void Container::OnChildHidden(Sprite &child)
{
	this->Sprite::OnChildHidden(child);

	Menu *menuPtr = dynamic_cast<Menu*>(&child);
	if ((menuPtr != NULL)
		&& menuPtr->HasToggleMode(UI_MTM_CENTRE_CURSOR)
		&& !HasVisibleChild())
	{
		// Reset the centre mechanism only when all children have been hidden.
		//mCursorCentred = false;
		ResetCursor();
	}
}

void Container::OnChildShown(Sprite &child)
{
	this->Sprite::OnChildShown(child);

	Menu *menuPtr = dynamic_cast<Menu*>(&child);
	if ((menuPtr != NULL)
		&& menuPtr->HasToggleMode(UI_MTM_CENTRE_CURSOR)
		&& !mCursorCentred)
	{
		CentreCursor(child);
	}
}

void Container::CentreCursor(Sprite &sprite)
{
	// Centre the cursor in the middle of the menu if required.
	VPoint3f cursor = GetUtility().GetCursorPositionSmooth();
	VPoint3f centre = (sprite.GetPosition() + (sprite.GetSize() / 2));
	mCursorOffset = cursor - centre;
	mCursorCentred = true;

}

void Container::ResetCursor()
{
	mCursorCentred = false;
	mCursorOffset = VPoint3f(0, 0, 0);
}

VPoint3f Container::GetCursorPosition() const
{
	return GetUtility().GetCursorPositionSmooth() - mCursorOffset;
}

}
}
