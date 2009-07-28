/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 20-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "MriSet.h"

using namespace vimrid::textures;

namespace vimrid
{
namespace mri
{

MriSet::MriSet()
{
}

MriSet::~MriSet()
{
	deleteData();
}

void MriSet::deleteData()
{
	for (VUInt32 i = 0; i < mSlices.size(); i++)
	{
		delete mSlices[i];
	}
}

MriSlice &MriSet::operator[](VUInt32 index)
{
	return *mSlices[index];
}

VUInt32 MriSet::SliceCount()
{
	return mSlices.size();
}

MriSlice *MriSet::AddTextureAsSlice(VTexture *texture)
{
	// Set the index as the slice count so rendering occurs in order of add.
	MriSlice *slice = new MriSlice(texture, mSlices.size());

	mSlices.push_back(slice);
	return slice;
}

void MriSet::Clear()
{
	deleteData();
	mSlices.clear();
}

}
}
