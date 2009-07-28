/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 22-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "VSize3F.h"

namespace vimrid
{

VSize3F::VSize3F() :
	Width(0.0),
	Height(0.0),
	Depth(0.0)
{
}

VSize3F::VSize3F(VFloat width, VFloat height) :
	Width(width),
	Height(height),
	Depth(0.0)
{
}

VSize3F::VSize3F(VFloat width, VFloat height, VFloat depth) :
	Width(width),
	Height(height),
	Depth(depth)
{
}

VSize3F::~VSize3F()
{
}

VSize3F VSize3F::operator/(const VFloat &f) const
{
	VSize3F s = *this;
	s.Width /= f;
	s.Height /= f;
	s.Depth /= f;
	return s;
}

}
