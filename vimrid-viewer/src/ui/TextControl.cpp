/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 02-May-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "TextControl.h"
#include <GL/glut.h>

using namespace std;

namespace vimrid
{
namespace ui
{

TextControl::TextControl(const string &text)
:
	mText(text),
	mFont(GLUT_BITMAP_HELVETICA_18),
	mTextOffset(0, 15)
{
}

TextControl::~TextControl()
{
}

void TextControl::OnSelectPress()
{
	mTextOffset += 1.0;
	this->Control::OnSelectPress();
}

void TextControl::OnSelectRelease()
{
	this->Control::OnSelectRelease();
}

void TextControl::OnSelectReset()
{
	mTextOffset -= 1.0;
	this->Control::OnSelectReset();
}

void TextControl::Update()
{
}

void TextControl::Render()
{
	ChangeColour(GetTextColour());
	glRasterPos2f(mTextOffset.X, mTextOffset.Y);
	for (string::iterator it = mText.begin(); it != mText.end(); it++)
	{
		glutBitmapCharacter(mFont, *it);
	}
}

VColour4F TextControl::GetTextColour() const
{
	if (IsEnabled())
	{
		if (IsHighlighted())
		{
			// Get a lighter bg on highlight.
			return mTextColour + 0.07;
		}
		else
		{
			// Just get normal background.
			return mTextColour;
		}
	}
	else
	{
		// Make text lighter (grayed out) to appear disabled.
		return mTextColour + 0.2;
	}
}

}
}
