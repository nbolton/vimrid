/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 07-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef VPOINT2F_H_
#define VPOINT2F_H_

#include "types.h"
#include <string>
#include <ostream>

namespace vimrid
{

class VPoint3f;
class VSize3F;

class VPoint2f
{
public:
	VPoint2f();
	VPoint2f(VFloat x, VFloat y);
	VPoint2f(const VPoint3f &p);
	virtual ~VPoint2f();
	VFloat X;
	VFloat Y;
	VPoint2f &operator=(const VPoint3f &rhs);
	VPoint2f operator+(const VPoint2f &rhs) const;
	VPoint2f operator-(const VPoint2f &rhs) const;
	VPoint2f operator+(const VPoint3f &rhs) const;
	VPoint2f operator-(const VPoint3f &rhs) const;
	VPoint2f operator+(const VSize3F &rhs) const;
	VPoint2f operator+(const VFloat &rhs) const;
	VPoint2f operator-(const VFloat &rhs) const;
	VPoint2f &operator+=(const VFloat &rhs);
	VPoint2f &operator-=(const VFloat &rhs);
	VPoint2f operator*(const VFloat &rhs) const;
	VBool operator>(const VPoint2f &rhs) const;
	VBool operator<(const VPoint2f &rhs) const;
	VBool operator>(const VPoint3f &rhs) const;
	VBool operator<(const VPoint3f &rhs) const;
	friend std::ostream &operator<<(std::ostream& os, const VPoint2f &point);
};

}

#endif // VPOINT2F_H_
