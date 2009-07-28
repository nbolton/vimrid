#include "ImageMatrix.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include "../Debug.h"

using namespace std;

namespace vimrid
{
namespace imaging
{

ImageMatrix::ImageMatrix()
{
}

ImageMatrix::ImageMatrix(
	VUInt32 rowCount,
	VUInt32 columnCount,
	VUInt32 channelCount)
	: VImage(rowCount, columnCount, channelCount)
{
	allocatePixelData();
}

ImageMatrix::ImageMatrix(const ImageMatrix &other) : VImage(other)
{
	allocatePixelData();
	LoadPixels(other.GetRawPixelData());
}

ImageMatrix::~ImageMatrix()
{
	delete[] mPixelData;
}

/*ImageMatrixRow &ImageMatrix::operator[](VUInt32 rowIndex)
{
	return rows[rowIndex];
}*/

void ImageMatrix::allocatePixelData()
{
	mPixelData = new VInt32[GetRowCount() * GetColumnCount() * GetChannelCount()];
}

//VInt32 &ImageMatrix::GetPixelAt(VUInt32 row, VUInt32 column, VUInt32 channel)
VInt32 &ImageMatrix::Get(VUInt32 row, VUInt32 column, VUInt32 channel) const
{
	//VUInt32 index = (column * GetColumnCount() + row) * GetChannelCount() + channel;
	VUInt32 index = (row * GetColumnCount() + column) * GetChannelCount() + channel;

	if (index >= GetPixelCount())
	{
		THROW_VIMRID_EX("Pixel index " << index << " (" <<
			row << ", " << column << ", " << channel << ") is out of bounds.");
	}

	//return rows[row][column][channel];
	return mPixelData[index];
}

VInt32 ImageMatrix::GetPixelMin() const
{
	VInt32 min = 0;
	for (VUInt32 row = 0; row < GetRowCount(); row++)
	{
		for (VUInt32 column = 0; column < GetColumnCount(); column++)
		{
			for (VUInt32 channel = 0; channel < GetChannelCount(); channel++)
			{
				//VInt32 pixel = GetPixelAt(row, column, channel);
				VInt32 pixel = Get(row, column, channel);
				if (pixel < min)
				{
					min = pixel;
				}
			}
		}
	}
	return min;
}

VInt32 ImageMatrix::GetPixelMax() const
{
	VInt32 max = 0;
	for (VUInt32 row = 0; row < GetRowCount(); row++)
	{
		for (VUInt32 column = 0; column < GetColumnCount(); column++)
		{
			for (VUInt32 channel = 0; channel < GetChannelCount(); channel++)
			{
				//VInt32 pixel = GetPixelAt(row, column, channel);
				VInt32 pixel = Get(row, column, channel);
				if (pixel > max)
				{
					max = pixel;
				}
			}
		}
	}
	return max;
}

ImageMatrix *ImageMatrix::CopyEmpty() const
{
	ImageMatrix *image = new ImageMatrix(
		GetRowCount(),
		GetColumnCount(),
		GetChannelCount());

	/* Allocated memory is not zero'd, so ensure all pixels are
	 * set to zero so that odd results dont occur in image processing.
	 */
	image->ZeroPixels();

	return image;
}

void ImageMatrix::ZeroPixels()
{
	for (VUInt32 i = 0; i < GetPixelCount(); i++)
	{
		mPixelData[i] = 0;
	}
}

VInt32 *ImageMatrix::CopyPixelsToInt32() const
{
	VInt32 *result = new VInt32[GetPixelCount()];
	VUInt32 pixelIndex = 0;

	for (VUInt32 row = 0; row < GetRowCount(); row++)
	{
		for (VUInt32 column = 0; column < GetColumnCount(); column++)
		{
			for (VUInt32 channel = 0; channel < GetChannelCount(); channel++)
			{
				//result[pixelIndex++] = GetPixelAt(row, column, channel);
				result[pixelIndex++] = Get(row, column, channel);
			}
		}
	}
	return result;
}

void ImageMatrix::_copyRotated(VDouble degrees, ImageMatrix &target) const
{
	// Use a reference as it's easier to type.
	//const ImageMatrix &source = *this;

	// Convert degree measurement to radians.
	VDouble angle = degrees / 57.3;

	// These are required so that we can rotate about the centre kernel.
	VDouble centerX = (VDouble)(GetColumnCount() - 1) / 2;
	VDouble centerY = (VDouble)(GetRowCount() - 1) / 2;

	// Calculate sin and cos outside of loop for better efficiency.
	VDouble angleCosine = cos(angle);
	VDouble angleSine = sin(angle);

	// Remember: row = y, column = x
	for (VUInt32 y = 0; y < GetRowCount(); y++)
	{
		for (VUInt32 x = 0; x < GetColumnCount(); x++)
		{
			/* Offset from the centre (which is re-added later), and use
			 * a double for convenience. */
	        VDouble dx = (VDouble)x - centerX;
	        VDouble dy = (VDouble)y - centerY;

			// Use rotation two matrix formula (and rotate about a specific point).
	        VDouble targetX = (angleCosine * dx) - (angleSine * dy) + centerX;
	        VDouble targetY = (angleCosine * dy) + (angleSine * dx) + centerY;

	        // Round the index so that it fits into the closest index.
	        int roundX = (int)round(targetX);
	        int roundY = (int)round(targetY);

			// Simply copy from the source image to the empty target.
	        //target[x][y][0] = source[roundX][roundY][0];
	        //VInt32 pixel = source.Get(roundX, roundY, 0);
	        VInt32 pixel = Get(roundX, roundY, 0);
	        target.Get(x, y, 0) = pixel;
		}
	}
}

void ImageMatrix::PrintDebugView() const
{
	for (VUInt32 row = 0; row < GetRowCount(); row++)
	{
		for (VUInt32 column = 0; column < GetColumnCount(); column++)
		{
			//cout << (*this)[row][column][0] << " ";
			cout << Get(row, column, 0) << " ";
		}
		cout << endl;
	}
}

void ImageMatrix::PrintDebugFile(const std::string &fileName) const
{
	ofstream file;
	file.open(fileName.c_str());
	for (VUInt32 row = 0; row < GetRowCount(); row++)
	{
		for (VUInt32 column = 0; column < GetColumnCount(); column++)
		{
			//file << (*this)[row][column][0] << " ";
			file << Get(row, column, 0) << " ";
		}
		file << endl;
	}
	file.close();
}

void ImageMatrix::assertImageDimensions(const ImageMatrix &otherImage) const
{
	const ImageMatrix &image1 = *this;
	const ImageMatrix &image2 = otherImage;

	if (image1.GetRowCount() != image2.GetRowCount())
	{
		THROW_VIMRID_EX(
			"The first matrix row count (" << image1.GetRowCount() <<
			") does not match that of the second (" << image2.GetRowCount() << ").");
	}

	if (image1.GetColumnCount() != image2.GetColumnCount())
	{
		THROW_VIMRID_EX(
			"The first matrix column count (" << image1.GetColumnCount() <<
			") does not match that of the second (" << image2.GetColumnCount() << ").");
	}

	if (image1.GetChannelCount() != image2.GetChannelCount())
	{
		THROW_VIMRID_EX(
			"The first matrix channel count (" << image1.GetChannelCount() <<
			") does not match that of the second (" << image2.GetChannelCount() << ").");
	}
}

void ImageMatrix::CombineWith(ImageMatrix &otherImage)
{
	assertImageDimensions(otherImage);

	for (VUInt32 row = 0; row < GetRowCount(); row++)
	{
		for (VUInt32 column = 0; column < GetRowCount(); column++)
		{
			//(*this)[row][column][0] += otherImage[row][column][0];
			Get(row, column, 0) += otherImage.Get(row, column, 0);
		}
	}
}

ImageMatrix *ImageMatrix::GetCombinedCopy(ImageMatrix &other) const
{
	assertImageDimensions(other);

	// Create empty result the same size as source.
	ImageMatrix &result = *CopyEmpty();

	VUInt32 srIndex; // Source row index
	VUInt32 scIndex; // Source column index

	VUInt32 srCount = GetRowCount(); // Source row count
	VUInt32 scCount = GetColumnCount(); // Source columns count

	for (srIndex = 0; srIndex < srCount; srIndex++)
	{
		for (scIndex = 0; scIndex < scCount; scIndex++)
		{
			/*VInt32 pixel1 = image1[srIndex][scIndex][0];
			VInt32 pixel2 = image2[srIndex][scIndex][0];

			result[srIndex][scIndex][0] = pixel1 + pixel2;*/

			VInt32 pixel1 = Get(srIndex, scIndex, 0);
			VInt32 pixel2 = other.Get(srIndex, scIndex, 0);
			result.Get(srIndex, scIndex, 0) = pixel1 + pixel2;
		}
	}

	return &result;
}

void ImageMatrix::Scope(VFloat scopeMin, VFloat scopeMax)
{
	VInt32 sourceMin = GetPixelMin();
	VInt32 sourceMax = GetPixelMax();
	VInt32 sourceRange = sourceMax - sourceMin;

	/* The target minimum is the the lowest number of pixels, plus the scope
	 * minimum, expressed as a fraction of the range. */
	VInt32 targetMin = sourceMin + (VInt32)round(sourceRange * scopeMin);

	/* The target maximum is the highest number of pixels, minus the scope
	 * maximum (but expressed as the difference from 1), expressed as a
	 * fraction of the range. */
	VInt32 targetMax = sourceMax - (VInt32)round(sourceRange * (1 - scopeMax));

	for (VUInt32 row = 0; row < GetRowCount(); row++)
	{
		for (VUInt32 column = 0; column < GetColumnCount(); column++)
		{
			//VInt32 sourcePixel = (*this)[row][column][0];
			VInt32 sourcePixel = Get(row, column, 0);

			if ((sourcePixel < targetMin) || (sourcePixel > targetMax))
			{
				// Truncate the pixel if it's outside of the scope.
				sourcePixel = 0;
			}

			//(*this)[row][column][0] = sourcePixel;
			Get(row, column, 0) = sourcePixel;
		}
	}
}

void ImageMatrix::Normalise(VInt32 targetMin, VInt32 targetMax)
{
	VInt32 sourceMin = GetPixelMin();
	VInt32 sourceMax = GetPixelMax();

	VDouble sourceRange = (VDouble)sourceMax - (VDouble)sourceMin;
	VDouble targetRange = (VDouble)targetMax - (VDouble)targetMin;

	// It's only worth scaling if there is actually a range to scale.
	if (sourceRange != 0)
	{
		for (VUInt32 row = 0; row < GetRowCount(); row++)
		{
			for (VUInt32 column = 0; column < GetColumnCount(); column++)
			{
				/* Get the original source pixel as a double, as it will be stored
				 * that way anyway since we're doing some division.
				 */
				//VDouble sourcePixel = (*this)[row][column][0];
				VDouble sourcePixel = Get(row, column, 0);

				/* Calculate scale which will be used to reduce/increase the actual,
				 * pixel to a value which fits appropriately between the target
				 * min and max values.
				 *
				 * TODO: Explain exactly how this works, not just what it's for.
				 */
				VDouble scale = (sourcePixel - sourceMin) / sourceRange;

				/* The calculated value will be a double, but it must be truncated
				 * to pixel value as this is the type that the matrix will represent.
				 *
				 * TODO: Consider changing matrix values to double, and then having
				 * a function that truncates them as integer (for better precision
				 * when processing the original pixel values several times, not sure
				 * if this will make a lot of difference to image quality).
				 *
				 * TODO: Explain exactly how this works, not just what it's for.
				 */
				//(*this)[row][column][0] = (VInt32)round(targetRange * scale) + targetMin;
				Get(row, column, 0) = (VInt32)round(targetRange * scale) + targetMin;
			}
		}
	}
}

ImageMatrix *ImageMatrix::GetNormalisedCopy(VInt32 targetMin, VInt32 targetMax) const
{
	ImageMatrix *copy = new ImageMatrix(*this);
	copy->Normalise(targetMin, targetMax);
	return copy;
}

void ImageMatrix::LoadPixels(const VInt32 *source)
{
	for (VUInt32 i = 0; i < GetPixelCount(); i++)
	{
		mPixelData[i] = source[i];
	}
}

}
}
