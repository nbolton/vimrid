#include "VImage.h"
#include <iostream>

using namespace std;

namespace vimrid
{
namespace imaging
{

VImage::VImage()
{
}

VImage::VImage(const VImage &other)
:
	mRowCount(other.mRowCount),
	mColumnCount(other.mColumnCount),
	mChannelCount(other.mChannelCount)
{
}

VImage::VImage(
	VUInt32 rowCount,
	VUInt32 columnCount,
	VUInt32 channelCount)
{
	if (rowCount == 0)
	{
		THROW_VIMRID_EX("Row count cannot be zero.");
	}

	if (columnCount == 0)
	{
		THROW_VIMRID_EX("Column count cannot be zero.");
	}

	if (channelCount == 0)
	{
		THROW_VIMRID_EX("Channel count cannot be zero.");
	}

	this->mRowCount = rowCount;
	this->mColumnCount = columnCount;
	this->mChannelCount = channelCount;
}

VImage::~VImage()
{
}

VUInt32 VImage::GetRowCount() const
{
	return mRowCount;
}

VUInt32 VImage::GetColumnCount() const
{
	return mColumnCount;
}

VUInt32 VImage::GetChannelCount() const
{
	return mChannelCount;
}

void VImage::SetRowCount(VUInt32 rowCount)
{
	this->mRowCount = rowCount;
}

void VImage::SetColumnCount(VUInt32 columnCount)
{
	this->mColumnCount = columnCount;
}

void VImage::SetChannelCount(VUInt32 channelCount)
{
	this->mChannelCount = channelCount;
}

VUInt32 VImage::GetPixelCount() const
{
	return mRowCount * mColumnCount * mChannelCount;
}

VUInt32 VImage::GetWidth() const
{
	return GetColumnCount();
}

VUInt32 VImage::GetHeight() const
{
	return GetRowCount();
}

/* Returns a reference to a character version of the original
 * pixel array.
 *
 * A warning is sent to cerr, when a pixel value is above 255.
 */
VUByte *VImage::CopyPixelsToUByte() const
{
	VInt32 *pixelInts = CopyPixelsToInt32();
	VUByte *pixelBytes = new VUByte[GetPixelCount()];

	for (VUInt32 i = 0; i < GetPixelCount(); i++)
	{
		// Copy the image pixels in to our own handler.
		VInt32 pixel = pixelInts[i];

		if (pixel > 255)
		{
			cerr << "Warning: Pixel value at index " << i <<
				" has value of " << pixel << " which will be truncated to 255." << endl;

			pixel = 255;
		}
		else if (pixel < 0)
		{
			// Not sure if this will ever happen.
			cerr << "Warning: Pixel value at index " << i <<
				" has value of " << pixel << " which will be truncated to 0." << endl;

			pixel = 0;
		}

		// Cast the pixel value (which will be <= 255) to a byte.
		pixelBytes[i] = (VUByte)pixel;
	}

	// Free up the int pixel once we have used them.
	delete [] pixelInts;

	return pixelBytes;
}

void VImage::UpdateDimnensions(
	VUInt32 rowCount,
	VUInt32 columnCount,
	VUInt32 channelCount)
{
	mRowCount = rowCount;
	mColumnCount = columnCount;
	mChannelCount = channelCount;
}

}
}
