/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 07-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "VPoint3f.h"
#include <sstream>

using namespace std;

namespace vimrid
{

VPoint3f::VPoint3f()
:
	X(0), Y(0), Z(0)
{
}

VPoint3f::VPoint3f(VFloat x, VFloat y, VFloat z)
:
	X(x), Y(y), Z(z)
{
}

VPoint3f::VPoint3f(const VPoint2f &p)
:
	X(p.X), Y(p.Y), Z(0.0)
{
}

VPoint3f::~VPoint3f()
{
}

VPoint3f VPoint3f::operator*(const VPoint3f &rhs) const
{
	VPoint3f value;
	const VPoint3f &lhs = *this;
	value.X = lhs.X * rhs.X;
	value.Y = lhs.Y * rhs.Y;
	value.Z = lhs.Z * rhs.Z;
	return value;
}

VPoint3f VPoint3f::operator*(const VFloat &rhs) const
{
	VPoint3f value;
	const VPoint3f &lhs = *this;
	value.X = lhs.X * rhs;
	value.Y = lhs.Y * rhs;
	value.Z = lhs.Z * rhs;
	return value;
}

VPoint3f &VPoint3f::operator=(const VPoint2f &rhs)
{
	X = rhs.X;
	Y = rhs.Y;
	return *this;
}

VPoint3f &VPoint3f::operator=(const VFloat &rhs)
{
	X = rhs;
	Y = rhs;
	Z = rhs;
	return *this;
}

VBool VPoint3f::operator!=(const VPoint3f &rhs)
{
	return (X != rhs.X) && (Y != rhs.Y) && (Z != rhs.Z);
}

VPoint3f VPoint3f::operator+(const VPoint3f &rhs) const
{
	VPoint3f value;
	const VPoint3f &lhs = *this;
	value.X = lhs.X + rhs.X;
	value.Y = lhs.Y + rhs.Y;
	value.Z = lhs.Z + rhs.Z;
	return value;
}

VPoint3f VPoint3f::operator-(const VPoint3f &rhs) const
{
	VPoint3f value;
	const VPoint3f &lhs = *this;
	value.X = lhs.X - rhs.X;
	value.Y = lhs.Y - rhs.Y;
	value.Z = lhs.Z - rhs.Z;
	return value;
}

VPoint3f VPoint3f::operator+(const VPoint2f &rhs) const
{
	VPoint3f value;
	const VPoint3f &lhs = *this;
	value.X = lhs.X + rhs.X;
	value.Y = lhs.Y + rhs.Y;
	return value;
}

VPoint3f VPoint3f::operator-(const VPoint2f &rhs) const
{
	VPoint3f value;
	const VPoint3f &lhs = *this;
	value.X = lhs.X - rhs.X;
	value.Y = lhs.Y - rhs.Y;
	return value;
}

VPoint3f VPoint3f::operator+(const VSize3F &rhs) const
{
	VPoint3f value;
	const VPoint3f &lhs = *this;
	value.X = lhs.X + rhs.Width;
	value.Y = lhs.Y + rhs.Height;
	value.Z = lhs.Z + rhs.Depth;
	return value;
}

VPoint3f VPoint3f::operator-(const VSize3F &rhs) const
{
	VPoint3f value;
	const VPoint3f &lhs = *this;
	value.X = lhs.X - rhs.Width;
	value.Y = lhs.Y - rhs.Height;
	value.Z = lhs.Z - rhs.Depth;
	return value;
}

VPoint3f VPoint3f::operator+(const VFloat &rhs) const
{
	VPoint3f value;
	const VPoint3f &lhs = *this;
	value.X = lhs.X + rhs;
	value.Y = lhs.Y + rhs;
	value.Z = lhs.Z + rhs;
	return value;
}

VPoint3f &VPoint3f::operator+=(const VPoint3f &rhs)
{
	X += rhs.X;
	Y += rhs.Y;
	Z += rhs.Z;
	return *this;
}

VPoint3f &VPoint3f::operator-=(const VPoint3f &rhs)
{
	X -= rhs.X;
	Y -= rhs.Y;
	Z -= rhs.Z;
	return *this;
}

VPoint3f &VPoint3f::operator+=(const VFloat &rhs)
{
	X += rhs;
	Y += rhs;
	Z += rhs;
	return *this;
}

VPoint3f &VPoint3f::operator-=(const VFloat &rhs)
{
	X -= rhs;
	Y -= rhs;
	Z -= rhs;
	return *this;
}

VPoint3f &VPoint3f::operator+=(const VSize3F &rhs)
{
	X += rhs.Width;
	Y += rhs.Height;
	Z += rhs.Depth;
	return *this;
}

VPoint3f &VPoint3f::operator-=(const VSize3F &rhs)
{
	X -= rhs.Width;
	Y -= rhs.Height;
	Z -= rhs.Depth;
	return *this;
}

VBool VPoint3f::operator>(const VPoint3f &rhs) const
{
	return (X > rhs.X) && (Y > rhs.Y) && (Z > rhs.Z);
}

VBool VPoint3f::operator<(const VPoint3f &rhs) const
{
	return (X < rhs.X) && (Y < rhs.Y) && (Z < rhs.Z);
}

VBool VPoint3f::operator>(const VPoint2f &rhs) const
{
	return (X > rhs.X) && (Y > rhs.Y);
}

VBool VPoint3f::operator<(const VPoint2f &rhs) const
{
	return (X < rhs.X) && (Y < rhs.Y);
}

VBool VPoint3f::operator>=(const VPoint3f &rhs) const
{
	return (X >= rhs.X) && (Y >= rhs.Y) && (Z >= rhs.Z);
}

VBool VPoint3f::operator<=(const VPoint3f &rhs) const
{
	return (X <= rhs.X) && (Y <= rhs.Y) && (Z <= rhs.Z);
}

ostream &operator<<(ostream& os, const VPoint3f &point)
{
	os << "X: " << point.X << " ";
	os << "Y: " << point.Y << " ";
	os << "Z: " << point.Z;
	return os;
}

}
