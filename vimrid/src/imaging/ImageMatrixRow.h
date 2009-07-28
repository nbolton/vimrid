#ifndef IMAGEMATRIXROW_H_
#define IMAGEMATRIXROW_H_

#include <map>
#include "../types/types.h"
#include "ImageMatrixColumn.h"

namespace vimrid
{
namespace imaging
{

class ImageMatrixRow
{
public:
	ImageMatrixColumn &operator[](VUInt32 columnIndex);
private:
	ImageMatrixColumnMap columns;
};

typedef std::map<VUInt32, ImageMatrixRow> ImageMatrixRowMap;

}
}

#endif // IMAGEMATRIXROW_H_
