/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 30-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "Menu.h"
#include <GL/gl.h>
#include <GL/glut.h>
#include <string>
#include "../glut/GlutInputManager.h"
#include "Container.h"
#include <math.h>

using namespace std;
using namespace vimrid::input;

namespace vimrid
{
namespace ui
{

Menu::Menu()
:
	mMenuButtonState(false),
	mLastMenuButtonState(false),
	mVisibleToggled(false),
	mToggleModes(0)
{
	// Hidden by default.
	Hide();

	// Use a dark gray background and set transparency.
	SetBackground(VColour4F(0.4, 0.4, 0.4, 0.8));
}

Menu::~Menu()
{
}

void Menu::Update()
{
	updatePosition();
	updateTrackdCursor();
	updateChildCollision();
	UpdateChildren();
	updateMenuToggled();
	updateVisible();
}

void Menu::updateMenuToggled()
{
	// By default, assume not suspended.
	VBool toggleSuspended = false;

	// Can only check for suspend if parent is container.
	Container *containerPtr = dynamic_cast<Container*>(GetParentPtr());
	if (containerPtr != NULL)
	{
		toggleSuspended = containerPtr->IsMenuToggleSuspended();
	}

	// Only detect and record toggle if toggle not suspended.
	if (!toggleSuspended)
	{
		if (GetUtility().IsTrackdEnabled())
		{
			mMenuButtonState = GetUtility().IsWandButtonPressed(TRACKD_BUTTON_2);
		}
		else
		{
			mMenuButtonState = GetUtility().IsGlutCharKeyPressed(GLUT_KEY_ESCAPE);
		}

		// Update the visible toggle flag before remembering the last state,
		mVisibleToggled = mMenuButtonState != mLastMenuButtonState;

		// Record last state so we can test for change next time.
		mLastMenuButtonState = mMenuButtonState;
	}
}

void Menu::updateVisible()
{
	if (mVisibleToggled)
	{
		if (mMenuButtonState && !IsVisible() && HasToggleMode(UI_MTM_SHOW))
		{
			Show();
		}
		else if (mMenuButtonState && IsVisible() && HasToggleMode(UI_MTM_HIDE))
		{
			Hide();
		}
	}
}

void Menu::OnShown()
{
	DEBUG("Menu shown: " << GetName());

	/*if (GetUtility().IsTrackdEnabled())
	{
		// HACK: Implement operator/ for VPoint2f!
		VPoint2f halfSize;
		halfSize.X = GetSize().Width / 2;
		halfSize.Y = GetSize().Height / 2;

		// Record cursor position when menu first shown (centre menu).
		mTrackdCursorStart = GetUtility().GetTrackdCursorPosition() - halfSize;
	}*/
	//mCursorStart = GetUtility().GetCursorPositionSmooth();

	this->Control::OnShown();
}

void Menu::OnHidden()
{
	DEBUG("Menu hidden: " << GetName());
	this->Control::OnHidden();
}

void Menu::updateChildCollision()
{
	const vector<Control*> &children = GetChildren();
	VBool selectButtonToggled = GetUtility().WasSelectButtonToggled();
	VBool selectButtonDown = GetUtility().IsSelectButtonPressed();
	VBool selectButtonPressed = selectButtonToggled && selectButtonDown;
	VBool selectButtonReleased = selectButtonToggled && !selectButtonDown;

	for (VUInt32 i = 0; i < children.size(); i++)
	{
		Control &child = *children[i];
		VBool collides = child.CollidesWith(getCursorPosition()) && child.IsVisible();

		// Cursor is over the control and it's not highlighted, so highlight.
		if (collides && !child.IsHighlighted() && child.IsEnabled())
		{
			child.Highlight();
		}

		// Cursor is not over the control and it's highlighted, so un-highlight.
		if (!collides && child.IsHighlighted() && child.IsEnabled())
		{
			child.Unhighlight();
		}

		// Select button has just been pressed, and cursor collides, so press.
		if (collides && selectButtonPressed && !child.IsSelected() && child.IsEnabled())
		{
			child.OnSelectPress();
		}

		// Select button has just been release, and cursor collides, so release.
		if (collides && selectButtonReleased && child.IsSelected() && child.IsEnabled())
		{
			child.OnSelectRelease();
		}

		/* Release when cursor has already left the control, as button
		 * may have been pressed during leavin the control's bounds.
		 */
		if (!collides && child.IsSelected() && child.IsEnabled())
		{
			child.OnSelectReset();
		}
	}
}

void Menu::updateTrackdCursor()
{
	/*if (GetUtility().IsTrackdEnabled())
	{
		mTrackdRelativeCursor = GetUtility().GetTrackdCursorPosition() - mTrackdCursorStart;
	}*/
}

void Menu::updatePosition()
{
	VInt32 x = (VInt32)round((GetUtility().GetWindowSize().Width - GetSize().Width) / 2);
	VInt32 y = (VInt32)round((GetUtility().GetWindowSize().Height - GetSize().Height) / 2);

	// Only apply the values if they're within screen range.
	VPoint2f position;
	if (x >= 0)
	{
		position.X = x;
	}
	if (y >= 0)
	{
		position.Y = y;
	}

	SetPosition(position);
}

void Menu::Render()
{
	TranslateToPosition();

	// Draw the menu background in the background colour.
	ChangeColour(GetBackground());
	DrawRectangle(VPoint2f(0, 0), GetSize());

	// Draw an outline with bevel effect so the menu appears to stand out.
	VSizeF outlineSize = VSizeF(GetSize().Width + 2, GetSize().Height + 2);
	DrawRectangleOutline(VPoint2f(-1, -1), outlineSize, true, false);

	RenderChildren();
	renderMouse();
}

void Menu::renderMouse()
{
	// Show the mouse cursor as brighter when clicked.
	if (GetUtility().IsSelectButtonPressed())
	{
		ChangeColour(VColour4F(1.0, 1.0, 1.0, 0.5));
	}
	else
	{
		ChangeColour(VColour4F(1.0, 1.0, 1.0, 0.3));
	}

	//DrawRectangle(getRelativeMouse(), VSizeF(10, 10));
	DrawRectangle(getCursorPosition(), VSizeF(10, 10));
}

VPoint2f Menu::getCursorPosition()
{
	Container *parentPtr = dynamic_cast<Container*>(GetParentPtr());
	if (parentPtr != NULL)
	{
		return parentPtr->GetCursorPosition() - GetPosition();
	}
	else
	{
		THROW_VIMRID_EX("Cannot get cursor position, parent is not Container type.");
	}

	/*
	VPoint3f p;
	VPoint3f cursorPosition = GetUtility().GetCursorPositionSmooth();
	if (GetUtility().IsTrackdEnabled())
	{
		//p = mTrackdRelativeCursor;

		// TODO: Only centre the cursor if first shown.
		p =  cursorPosition;// + (GetSize() / 2);
	}
	else
	{
		//p = getRelativeMouse();

		//return cursorPosition - GetPosition();
		//return cursorPosition + (GetSize() / 2);
		p = cursorPosition;// - mCursorStart;
	}

	// Only centre the cursor if the menu is required to do so.
	VSize3F centre;
	if (HasToggleMode(UI_MTM_CENTRE_CURSOR))
	{
		centre = (GetSize() / 2);
	}

	p -= mCursorStart - centre;

	return p;*/
}

VPoint2f Menu::getRelativeMouse()
{
	VPoint2f p;
	p = GetUtility().GetMousePosition() - GetPosition();
	return p;
}

void Menu::AddToggleMode(MenuToggleMode toggleMode)
{
	if (HasToggleMode(toggleMode))
	{
		THROW_VIMRID_EX("Toggle mode (" << toggleMode << ") is already added.");
	}
	mToggleModes |= (VInt32)toggleMode;
}

void Menu::RemoveToggleMode(MenuToggleMode toggleMode)
{
	if (!HasToggleMode(toggleMode))
	{
		THROW_VIMRID_EX("Toggle mode (" << toggleMode << ") was never added and so cannot be removed.");
	}
	mToggleModes ^= (VInt32)toggleMode;
}

VBool Menu::HasToggleMode(MenuToggleMode toggleMode)
{
	return (mToggleModes & toggleMode) != 0;
}

}
}
