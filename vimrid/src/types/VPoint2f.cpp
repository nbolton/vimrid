/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 07-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "VPoint2f.h"
#include <sstream>

using namespace std;

namespace vimrid
{

VPoint2f::VPoint2f()
:
	X(0), Y(0)
{
}

VPoint2f::VPoint2f(VFloat x, VFloat y)
:
	X(x), Y(y)
{
}

VPoint2f::VPoint2f(const VPoint3f &p)
:
	X(p.X), Y(p.Y)
{
}

VPoint2f::~VPoint2f()
{
}

VPoint2f &VPoint2f::operator=(const VPoint3f &rhs)
{
	X = rhs.X;
	Y = rhs.Y;
	return *this;
}

VPoint2f VPoint2f::operator+(const VPoint2f &rhs) const
{
	VPoint2f value;
	const VPoint2f &lhs = *this;
	value.X = lhs.X + rhs.X;
	value.Y = lhs.Y + rhs.Y;
	return value;
}

VPoint2f VPoint2f::operator-(const VPoint2f &rhs) const
{
	VPoint2f value;
	const VPoint2f &lhs = *this;
	value.X = lhs.X - rhs.X;
	value.Y = lhs.Y - rhs.Y;
	return value;
}

VPoint2f VPoint2f::operator+(const VPoint3f &rhs) const
{
	VPoint2f value;
	const VPoint2f &lhs = *this;
	value.X = lhs.X + rhs.X;
	value.Y = lhs.Y + rhs.Y;
	return value;
}

VPoint2f VPoint2f::operator-(const VPoint3f &rhs) const
{
	VPoint2f value;
	const VPoint2f &lhs = *this;
	value.X = lhs.X - rhs.X;
	value.Y = lhs.Y - rhs.Y;
	return value;
}

VPoint2f VPoint2f::operator+(const VSize3F &rhs) const
{
	VPoint2f value;
	const VPoint2f &lhs = *this;
	value.X = lhs.X + rhs.Width;
	value.Y = lhs.Y + rhs.Height;
	return value;
}

VPoint2f VPoint2f::operator+(const VFloat &rhs) const
{
	VPoint2f value;
	const VPoint2f &lhs = *this;
	value.X = lhs.X + rhs;
	value.Y = lhs.Y + rhs;
	return value;
}

VPoint2f VPoint2f::operator-(const VFloat &rhs) const
{
	VPoint2f value;
	const VPoint2f &lhs = *this;
	value.X = lhs.X - rhs;
	value.Y = lhs.Y - rhs;
	return value;
}

VPoint2f &VPoint2f::operator+=(const VFloat &rhs)
{
	X += rhs;
	Y += rhs;
	return *this;
}

VPoint2f &Point2f::operator-=(const VFloat &rhs)
{
	X -= rhs;
	Y -= rhs;
	return *this;
}

VPoint2f Point2f::operator*(const VFloat &rhs) const
{
	VPoint2f value;
	const VPoint2f &lhs = *this;
	value.X = lhs.X * rhs;
	value.Y = lhs.Y * rhs;
	return value;
}

VBool VPoint2f::operator>(const VPoint2f &rhs) const
{
	return (X > rhs.X) && (Y > rhs.Y);
}

VBool VPoint2f::operator<(const VPoint2f &rhs) const
{
	return (X < rhs.X) && (Y < rhs.Y);
}

VBool VPoint2f::operator>(const VPoint3f &rhs) const
{
	return (X > rhs.X) && (Y > rhs.Y);
}

VBool VPoint2f::operator<(const VPoint3f &rhs) const
{
	return (X < rhs.X) && (Y < rhs.Y);
}

ostream &operator<<(ostream& os, const VPoint2f &point)
{
	os << "X: " << point.X << " ";
	os << "Y: " << point.Y;
	return os;
}

}
