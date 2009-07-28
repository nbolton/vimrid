/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 01-May-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "Control.h"
#include <GL/gl.h>

#define DEFAULT_NAME "unnamed"

using namespace std;

namespace vimrid
{
namespace ui
{

Control::Control()
:
	mName(DEFAULT_NAME),
	mSelected(false),
	mEnabled(true),
	mHighlighted(false)
{
}

Control::Control(VUInt32 renderIndex)
:
	Sprite(renderIndex),
	mName(DEFAULT_NAME),
	mSelected(false),
	mEnabled(true),
	mHighlighted(false)
{
}

Control::~Control()
{
	for (VUInt32 i = 0; i < mChildren.size(); i++)
	{
		delete mChildren[i];
	}
}

void Control::OnSelectPress()
{
	mSelected = true;

	if (HasParentControl())
	{
		// If possible tell parent select has been pressed.
		GetParentControl().OnChildSelectPress(*this);
	}
}

void Control::OnSelectRelease()
{
	OnSelectReset();

	if (HasParentControl())
	{
		// If possible tell parent select has been released.
		GetParentControl().OnChildSelectRelease(*this);
	}
}

void Control::OnSelectReset()
{
	mSelected = false;
}

void Control::AddChild(Control &child)
{
	// Use a parent so child can do things like close the parent on click.
	child.SetParent(*this);

	// Add to map and vector so its both easy to get at index, and easy to iterate.
	mChildren.push_back(&child);
}

void Control::UpdateChildren()
{
	for (VUInt32 i = 0; i < mChildren.size(); i++)
	{
		Control &child = *mChildren[i];
		child.TriggerUpdate();
	}
}

void Control::RenderChildren()
{
	for (VUInt32 i = 0; i < mChildren.size(); i++)
	{
		Control &child = *mChildren[i];
		if (child.IsVisible())
		{
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();

			child.TriggerRender();

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		}
	}
}

void Control::DrawRectangle(const VPoint2f &position, const VSizeF &size) const
{
	glBegin(GL_QUADS);
	glVertex2f(position.X, position.Y);
	glVertex2f(position.X, position.Y + size.Height);
	glVertex2f(position.X + size.Width, position.Y + size.Height);
	glVertex2f(position.X + size.Width, position.Y);
	glEnd();
}

void Control::DrawRectangleOutline(
	const VPoint2f &position,
	const VSizeF &size,
	VBool bevelColour,
	VBool invertBevel) const
{
	VPoint2f topLeft = position;
	VPoint2f bottomLeft = position + VPoint2f(0, size.Height);
	VPoint2f bottomRight = position + size;
	VPoint2f topRight = position + VPoint2f(size.Width, 0);

	VColour4F lightEdge = GetBackground() + (invertBevel ? -.1 : .1);
	VColour4F darkEdge = GetBackground() + (invertBevel ? .1 : -.1);

	glBegin(GL_LINES);

	if (bevelColour)
		ChangeColour(lightEdge);

	// Top left -> bottom left
	glVertex2f(topLeft.X, topLeft.Y);
	glVertex2f(bottomLeft.X, bottomLeft.Y);

	if (bevelColour)
		ChangeColour(darkEdge);

	// Bottom left -> bottom right
	glVertex2f(bottomLeft.X + 1, bottomLeft.Y);
	glVertex2f(bottomRight.X - 1, bottomRight.Y);

	if (bevelColour)
		ChangeColour(darkEdge);

	// Bottom right -> top right
	glVertex2f(bottomRight.X - 1, bottomRight.Y);
	glVertex2f(topRight.X, topRight.Y);

	if (bevelColour)
		ChangeColour(lightEdge);

	// Top right -> top left
	glVertex2f(topRight.X - 1, topRight.Y + 1);
	glVertex2f(topLeft.X + 1, topLeft.Y + 1);

	glEnd();
}

Control *Control::getParentControlPtr()
{
	return dynamic_cast<Control*>(GetParentPtr());
}

VBool Control::HasParentControl()
{
	return getParentControlPtr() != NULL;
}

Control &Control::GetParentControl()
{
	if (!HasParentControl())
	{
		THROW_VIMRID_EX("Parent is either not set, or not of Control type.");
	}
	return *getParentControlPtr();
}

void Control::OnChildSelectPress(Control &child)
{
	if (HasParentControl())
	{
		GetParentControl().OnChildSelectPress(child);
	}
}

void Control::OnChildSelectRelease(Control &child)
{
	if (HasParentControl())
	{
		GetParentControl().OnChildSelectRelease(child);
	}
}

VBool Control::HasVisibleChild()
{
	VBool foundVisible = false;
	for (size_t i = 0; i < mChildren.size(); i++)
	{
		if (mChildren[i]->IsVisible())
		{
			foundVisible = true;
		}
	}
	return foundVisible;
}

VColour4F Control::GetBackground() const
{
	if (IsEnabled())
	{
		if (IsHighlighted())
		{
			// Get a lighter bg on highlight.
			return mBackground + 0.07;
		}
		else
		{
			// Just get normal background.
			return mBackground;
		}
	}
	else
	{
		// Make darker to appear disabled.
		return mBackground - 0.2;
	}
}

}
}
