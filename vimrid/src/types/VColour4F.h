/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 03-May-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef VCOLOUR4F_H_
#define VCOLOUR4F_H_

#include "types.h"

namespace vimrid
{

class VColour4F
{
public:
	VColour4F();
	VColour4F(
		VFloat red,
		VFloat green,
		VFloat blue,
		VFloat alpha);
	virtual ~VColour4F();
	VFloat Red;
	VFloat Green;
	VFloat Blue;
	VFloat Alpha;
	VColour4F operator+(const VFloat &rhs) const;
	VColour4F operator-(const VFloat &rhs) const;
};

}

#endif // VCOLOUR4F_H_
