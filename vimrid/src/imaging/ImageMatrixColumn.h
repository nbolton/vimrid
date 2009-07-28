#ifndef IMAGEMATRIXCOLUMN_H_
#define IMAGEMATRIXCOLUMN_H_

#include <map>
#include "../types/types.h"

namespace vimrid
{
namespace imaging
{

typedef std::map <VUInt32, VInt32> ImageMatrixChannelMap;

class ImageMatrixColumn
{
public:
	VInt32 &operator[](VUInt32 channelIndex);
private:
	ImageMatrixChannelMap channels;
};

typedef std::map<VUInt32, ImageMatrixColumn> ImageMatrixColumnMap;

}
}

#endif // IMAGEMATRIXCOLUMN_H_
