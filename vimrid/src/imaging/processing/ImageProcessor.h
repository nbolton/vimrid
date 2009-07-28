/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 09-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef IMAGEPROCESSOR_H_
#define IMAGEPROCESSOR_H_

#include "../VImage.h"
#include "ImageFilter.h"

namespace vimrid
{
namespace imaging
{
namespace processing
{

class ImageProcessor
{
public:
	ImageProcessor(const ImageMatrix &image, const ImageFilter &filter);
	virtual ~ImageProcessor();
	ImageMatrix *ProcessImage();
	bool InvertFilter;
	bool NormaliseResponse;
	VUInt32 FilterRotations;
	VFloat ScopeMin;
	VFloat ScopeMax;
	VInt32 NormalMin;
	VInt32 NormalMax;
	VFloat StartAngle;
private:
	const ImageMatrix &mImage;
	const ImageFilter mFilter;
};

}
}
}

#endif // IMAGEPROCESSOR_H_
