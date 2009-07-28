/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 07-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef VPOINT3F_H_
#define VPOINT3F_H_

#include "types.h"
#include <string>
#include <ostream>

namespace vimrid
{

class VPoint2f;
class VSize3F;

class VPoint3f
{
public:
	VPoint3f();
	VPoint3f(VFloat x, VFloat y, VFloat z);
	VPoint3f(const VPoint2f &p);
	virtual ~VPoint3f();
	VFloat X;
	VFloat Y;
	VFloat Z;
	VPoint3f &operator=(const VPoint2f &rhs);
	VPoint3f &operator=(const VFloat &rhs);
	VBool operator!=(const VPoint3f &rhs);
	VPoint3f operator+(const VPoint3f &rhs) const;
	VPoint3f operator-(const VPoint3f &rhs) const;
	VPoint3f operator+(const VPoint2f &rhs) const;
	VPoint3f operator-(const VPoint2f &rhs) const;
	VPoint3f operator+(const VSize3F &rhs) const;
	VPoint3f operator-(const VSize3F &rhs) const;
	VPoint3f operator+(const VFloat &rhs) const;
	VPoint3f &operator+=(const VPoint3f &rhs);
	VPoint3f &operator-=(const VPoint3f &rhs);
	VPoint3f &operator+=(const VFloat &rhs);
	VPoint3f &operator-=(const VFloat &rhs);
	VPoint3f &operator+=(const VSize3F &rhs);
	VPoint3f &operator-=(const VSize3F &rhs);
	VPoint3f operator*(const VPoint3f &rhs) const;
	VPoint3f operator*(const VFloat &rhs) const;
	VBool operator>(const VPoint3f &rhs) const;
	VBool operator<(const VPoint3f &rhs) const;
	VBool operator>(const VPoint2f &rhs) const;
	VBool operator<(const VPoint2f &rhs) const;
	VBool operator>=(const VPoint3f &rhs) const;
	VBool operator<=(const VPoint3f &rhs) const;
	friend std::ostream &operator<<(std::ostream& os, const VPoint3f &point);
};

}

#endif // VPOINT3F_H_
