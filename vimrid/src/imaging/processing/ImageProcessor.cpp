/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 09-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "ImageProcessor.h"
#include "../../Debug.h"
#include <vector>

using namespace std;

namespace vimrid
{
namespace imaging
{
namespace processing
{

ImageProcessor::ImageProcessor(const ImageMatrix &image, const ImageFilter &filter) :
	InvertFilter(false),
	NormaliseResponse(true),
	FilterRotations(2),
	ScopeMin(0),
	ScopeMax(1),
	NormalMin(0),
	NormalMax(255),
	StartAngle(0),
	mImage(image),
	mFilter(filter)
{
}

ImageProcessor::~ImageProcessor()
{
}

ImageMatrix *ImageProcessor::ProcessImage()
{
	// Create a final response, and just keep adding to it.
	ImageMatrix &finalImage = *mImage.CopyEmpty();

	/* In default mode, rotate as many times as is necessary up to 90 degrees
	 * so that the filter covers as many angles as is needed. At each rotation,
	 * record the filter response and then move on to the next. When in invert
	 * filter mode, start from 180 instead so that it starts off with the exact
	 * mirror of the default filter prototype. */
	VDouble angle = StartAngle + (InvertFilter ? 180 : 0);
	for (VUInt32 i = 0; i < FilterRotations; i++)
	{
		WRITE_DEBUG_TEXT("Rotating filter to " << angle << " degrees...");
		ImageFilter &rotatedFilter = *mFilter.GetRotatedCopy<ImageFilter>(angle);

		WRITE_DEBUG_TEXT("Convoluting image with filter...");
		ImageMatrix &response = *rotatedFilter.ConvoluteImage(mImage);

		// Finished with the rotated filter.
		delete &rotatedFilter;

		WRITE_DEBUG_TEXT("Combining filter response with final image...");
		finalImage.CombineWith(response);

		// Safe to delete this response, as it has been combined with final.
		delete &response;

		if (FilterRotations > 1)
		{
			/* If only one rotation is needed, then there's no need to increase,
			 * also if the division were to occur a divide by zero would happen.
			 * If for example two rotations are needed, then 90 is added to the
			 * angle, making the first rotation at 0, and the second at 90.
			 * If however three are required, the first is rotated by 0, then
			 * 45 (90 / 2) is added, finally resulting in a 3rd rotation of 90.
			 */
			angle += (90 / (FilterRotations - 1));
		}
	}

	if ((ScopeMin != 0) || (ScopeMax != 1))
	{
		WRITE_DEBUG_TEXT("Scoping final image...");
		finalImage.Scope(ScopeMin, ScopeMax);
	}

	if (NormaliseResponse)
	{
		WRITE_DEBUG_TEXT("Normalising final image...");
		finalImage.Normalise(NormalMin, NormalMax);
	}

	return &finalImage;
}

}
}
}
