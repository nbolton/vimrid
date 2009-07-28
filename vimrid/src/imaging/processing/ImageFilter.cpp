/*
 * ImageFilter.cpp
 *
 *  Created on: 06-Apr-2009
 *      Author: nick
 */

#include "ImageFilter.h"
#include <math.h>

namespace vimrid
{
namespace imaging
{
namespace processing
{

ImageFilter::ImageFilter(
	VUInt32 rowCount,
	VUInt32 columnCount,
	VUInt32 channelCount)
	: ImageMatrix(rowCount, columnCount, channelCount)
{
}

ImageFilter::~ImageFilter()
{
}

/* Reverse engineered from Filter.java which was
 * written by Wenda He (Aberystwyth University).
 */
ImageMatrix *ImageFilter::ConvoluteImage(const ImageMatrix &source) const
{
	const ImageFilter &filter = *this;

	// Create empty result the same size as source.
	ImageMatrix &result = *source.CopyEmpty();

	VUInt32 srIndex; // Source row index
	VUInt32 scIndex; // Source column index
	VInt32 frIndex; // Filter row index
	VInt32 fcIndex; // Filter column index

	VUInt32 srCount = source.GetRowCount(); // Source row count
	VUInt32 scCount = source.GetColumnCount(); // Source columns count
	VUInt32 frCount = filter.GetRowCount(); // Filter row count
	VUInt32 fcCount = filter.GetColumnCount(); // Filter column count

	// Run the filter over each pixel in the image.
	for (srIndex = 0; srIndex < srCount; srIndex++)
	{
		for (scIndex = 0; scIndex < scCount; scIndex++)
		{
			/* The convoluted pixel to be stored; the result of each
			 * matrix transformation is accumulated on each value of the
			 * filter matrix.
			 */
			VInt32 filterResponsePixel = 0;

			/* TODO: Explain why are we starting at minus values...
			 *
			 * Correct:?
			 * Don't start the filter at the very top left, but start inward
			 * at half of the filter size so that we don't touch the boundary
			 * of the image (as this wouldn't look very nice).
			 */
			VInt32 frStart = -(VInt32)((VFloat)frCount / 2);
			VInt32 fcStart = -(VInt32)((VFloat)fcCount / 2);
			VInt32 frEnd = (VInt32)((VFloat)frCount / 2);
			VInt32 fcEnd = ((VInt32)(VFloat)fcCount / 2);

			/* For each value in the filter matrix, apply convolution
			 * and accumulate the result for each pixel.
			 */
			for (frIndex = frStart; frIndex <= frEnd; frIndex++)
			{
				for (fcIndex = fcStart; fcIndex <= fcEnd; fcIndex++)
				{
					/* Calculate the indices to be used for accessing the
					 * source image pixels.
					 *
					 * TODO: Explain the first 2 blocks of code. Why is so
					 * much adding and subtraction going on? Is there a simpler
					 * way of doing the same thing?
					 */

					VInt32 rowIndex = srIndex + frIndex;
					if (rowIndex < 0)
					{
						rowIndex += srCount;
					}
					else if (rowIndex >= (VInt32)srCount)
					{
						rowIndex -= srCount;
					}

					VInt32 columnIndex = scIndex + fcIndex;
					if (columnIndex < 0)
					{
						columnIndex += scCount;
					}
					else if (columnIndex >= (VInt32)scCount)
					{
						columnIndex -= scCount;
					}

					/* Calculate the indices to be used for accessing the filter value.
					 *
					 * TODO: Explain the arithmetic used; i.e. why is the index
					 * added to the total columns then divided by 2...
					 */
					VInt32 useFrIndex = frIndex + frCount / 2;
					VInt32 useFcIndex = fcIndex + fcCount / 2;

					/* Get the source image pixel and filter value to be applied;
					 * nothing complicated here, just declaring these as variables
					 * to make the code more readable.
					 */
					//VDouble sourcePixel = (VDouble)source[rowIndex][columnIndex][0];
					//VDouble filterValue = (VDouble)filter[useFrIndex][useFcIndex][0];
					VDouble sourcePixel = (VDouble)source.Get(rowIndex, columnIndex, 0);
					VDouble filterValue = (VDouble)filter.Get(useFrIndex, useFcIndex, 0);

					/* Multiply the pixel value with each value of the filter matrix;
					 * because we want the result of the entire matrix against each
					 * pixel, the value is accumulated and set to the result image.
					 */
					VDouble convolvingValue = sourcePixel * filterValue;
					filterResponsePixel += (VInt32)round(convolvingValue);
				}
			}

			// After the convolution on this particular pixel, set the result.
			//result[srIndex][scIndex][0] = filterResponsePixel;
			result.Get(srIndex, scIndex, 0) = filterResponsePixel;
		}
	}
	return &result;
}

}
}
}
