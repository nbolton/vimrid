/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 03-May-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "VColour4F.h"

namespace vimrid
{

VColour4F::VColour4F()
:
	Red(0),
	Green(0),
	Blue(0),
	Alpha(0)
{
}

VColour4F::VColour4F(
	VFloat red,
	VFloat green,
	VFloat blue,
	VFloat alpha)
:
	Red(red),
	Green(green),
	Blue(blue),
	Alpha(alpha)
{
}

VColour4F::~VColour4F()
{
}

VColour4F VColour4F::operator+(const VFloat &rhs) const
{
	VColour4F value;
	const VColour4F &lhs = *this;
	value.Red = lhs.Red + rhs;
	value.Green = lhs.Green + rhs;
	value.Blue = lhs.Blue + rhs;
	value.Alpha = lhs.Alpha + rhs;
	return value;
}

VColour4F VColour4F::operator-(const VFloat &rhs) const
{
	VColour4F value;
	const VColour4F &lhs = *this;
	value.Red = lhs.Red - rhs;
	value.Green = lhs.Green - rhs;
	value.Blue = lhs.Blue - rhs;
	value.Alpha = lhs.Alpha - rhs;
	return value;
}

}
