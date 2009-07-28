/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 22-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef VSIZE3F_H_
#define VSIZE3F_H_

#include "types.h"

namespace vimrid
{

class VSize3F
{
public:
	VSize3F();
	VSize3F(VFloat width, VFloat height);
	VSize3F(VFloat width, VFloat height, VFloat depth);
	virtual ~VSize3F();
	VFloat Width;
	VFloat Height;
	VFloat Depth;
	VSize3F operator/(const VFloat &f) const;
};

}

#endif // VSIZE3F_H_
