/*
 * ImageFilter.h
 *
 *  Created on: 06-Apr-2009
 *      Author: nick
 */

#ifndef IMAGEFILTER_H_
#define IMAGEFILTER_H_

#include "../VImage.h"
#include "../ImageMatrix.h"

namespace vimrid
{
namespace imaging
{
namespace processing
{

class ImageFilter : public ImageMatrix
{
public:
	virtual ~ImageFilter();
	ImageMatrix *ConvoluteImage(const ImageMatrix &source) const;
protected:
	ImageFilter(
		VUInt32 rowCount,
		VUInt32 columnCount,
		VUInt32 channelCount);
};

}
}
}

#endif // IMAGEFILTER_H_
