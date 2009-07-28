#include "ImageMatrixRow.h"

namespace vimrid
{
namespace imaging
{

ImageMatrixColumn &ImageMatrixRow::operator[](VUInt32 columnIndex)
{
	return columns[columnIndex];
}

}
}
