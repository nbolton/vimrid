#include "ImageMatrixColumn.h"

namespace vimrid
{
namespace imaging
{

VInt32 &ImageMatrixColumn::operator[](VUInt32 channelIndex)
{
	return channels[channelIndex];
}

}
}
