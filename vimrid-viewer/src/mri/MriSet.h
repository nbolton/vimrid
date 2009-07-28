/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 20-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef MRISET_H_
#define MRISET_H_

#include <vector>
#include "MriSlice.h"
#include "../textures/VTexture.h"

namespace vimrid
{
namespace mri
{

class MriSet
{
public:
	MriSet();
	virtual ~MriSet();
	VUInt32 SliceCount();
	MriSlice &operator[](VUInt32 index);
	MriSlice *AddTextureAsSlice(vimrid::textures::VTexture *texture);
	void Clear();
	std::vector<MriSlice*> &GetSlices() { return mSlices; }
private:
	std::vector<MriSlice*> mSlices;
	void deleteData();
};

}
}

#endif // MRISET_H_
