/* Stores information about a DICOM image.
 *
 * Pixels are stored in rows, and should be read in this fashion:
 * row 0 > column 0 > channel 0 ... n > column 1 .. n > row 1 ... n
 *
 * Pixels are stored as 32-bit integers, as the DICOM standard supports
 * gray levels over 255. This class supports conversion to byte, which
 * first reduces the brightness so that most values fall below 255, then
 * truncates any remaining values over 255 to 255.
 *
 * HACK: - I think I fixed this??
 * DICOM supports pixel values over 8-bit (e.g. 16-bit), and
 * just turning down the brightness by a percentage is a rather
 * hacky approach. The bit-level of the pixels should be stored
 * in the object creation and serialisation, and the pixel values
 * should be "translated".
 */

#include "DicomImage.h"
#include "../VimridException.h"
#include <stdlib.h>
#include <iostream>
#include <math.h>

using namespace std;
using namespace vimrid::imaging;

namespace vimrid
{
namespace dicom
{

string DicomImage::EndFile = "EndFile;";
string DicomImage::EndStream = "EndStream;";
string DicomImage::Error = "Error;";

DicomImage::DicomImage(
	VUInt32 rowCount,
	VUInt32 columnCount,
	VUInt32 channelCount,
	VByte bitLength,
	VInt32 *pixelData)
:
	VImage(rowCount, columnCount, channelCount),
	mBitLength(bitLength),
	mPixelData(pixelData)
{
}

DicomImage::DicomImage(const DicomImage &other)
:
	VImage(other),
	mBitLength(other.mBitLength)
{
	// Copy the pixel values from one to the other.
	allocatePixelData();
	for (VUInt32 i = 0; i < GetPixelCount(); i++)
	{
		mPixelData[i] = other.mPixelData[i];
	}
}

DicomImage::DicomImage(string serialised)
{
	SetRowCount(stringToInt(getValueFromSerial(serialised, "Rows")));
	SetColumnCount(stringToInt(getValueFromSerial(serialised, "Columns")));
	SetChannelCount(stringToInt(getValueFromSerial(serialised, "Channels")));
	mBitLength = stringToInt(getValueFromSerial(serialised, "BitLength"));
	mPixelData = deserialisePixels(getValueFromSerial(serialised, "Pixels"));
}

DicomImage::~DicomImage()
{
	delete[] mPixelData;
}

void DicomImage::allocatePixelData()
{
	mPixelData = new VInt32[GetRowCount() * GetColumnCount() * GetChannelCount()];
}

VByte DicomImage::GetBitLength()
{
	return mBitLength;
}

string DicomImage::getValueFromSerial(string serialised, string name)
{
	// Find where the name starts.
	int nameStart = serialised.find(name);
	if (nameStart == -1)
	{
		THROW_VIMRID_EX("Could not find value with name \"" << name << "\".");
	}

	// The value starts after the colon after the name.
	int valueStart = nameStart + name.length() + 1;

	// Find the next occurrence of the terminator, from the start of the value.
	int valueEnd = serialised.find(";", valueStart);
	if (valueEnd == -1)
	{
		THROW_VIMRID_EX("Could not find value delimiter (header is corrupt).");
	}

	// Return the value string (where the length is end - start).
	return serialised.substr(valueStart, (valueEnd - valueStart));
}

VUInt32 DicomImage::stringToInt(string s)
{
	return atoi(s.c_str());
}

string DicomImage::GetSerialised()
{
	stringstream stream;
	stream << "Rows:" << GetRowCount() << ";";
	stream << "Columns:" << GetColumnCount() << ";";
	stream << "Channels:" << GetChannelCount() << ";";

	// Use the number equivalent, not the character.
	stream << "BitLength:" << (VInt32)mBitLength << ";";

	stream << "Pixels:" << getSerialisedPixels() << ";";
	stream << DicomImage::EndFile;
	return stream.str();
}

VInt32 *DicomImage::deserialisePixels(string serialised)
{
	VUInt32 pixelCount = GetPixelCount();
	if (pixelCount == 0)
	{
		THROW_VIMRID_EX(
			"Cannot deserialise a serialised pixel array when "
			<< "none of column, row or channel counts are 0.");
	}

	VUInt32 index = 0;
	VInt32 *pixels = new VInt32[pixelCount];

	for (VUInt32 i = 0; i < pixelCount; i++)
	{
		// Find where the value is delimited, so we know where to substr up to.
		VUInt32 valueEnd = serialised.find(",", index);

		// Get the string value of the pixel based on the index and delimited position.
		string pixelString = serialised.substr(index, (valueEnd - index));

		// Convert the string to an int and store in the pixels array.
		pixels[i] = atoi(pixelString.c_str());

		// Advance the index to the position of the next value.
		index += pixelString.length() + 1;
	}

	return pixels;
}

string DicomImage::getSerialisedPixels()
{
	stringstream stream;
	for (VUInt32 i = 0; i < GetPixelCount(); i++)
	{
		stream << mPixelData[i] << ",";
	}
	return stream.str();
}

/* Returns a copy of the original pixels which can be modified, the
 * original pointer is not returned to maintain ownership to this
 * class. The pointer returned by this function must be deleted after
 * use.
 */
VInt32 *DicomImage::CopyPixelsToInt32() const
{
	VInt32 *pixels = new VInt32[GetPixelCount()];
	for (VUInt32 i = 0; i < GetPixelCount(); i++)
	{
		pixels[i] = mPixelData[i];
	}
	return pixels;
}

void DicomImage::ChangeBitLength(VByte targetBitLength)
{
	// Only change the bit length if different.
	if (this->mBitLength != targetBitLength)
	{
		// Calculate the factor to which the old pixel will be multiplied.
		VFloat convertFactor = (VFloat)targetBitLength / (VFloat)this->mBitLength;

		for (VUInt32 i = 0; i < GetPixelCount(); i++)
		{
			// Store the old value so we can do some arithmetic.
			VInt32 pixel = mPixelData[i];

			// Replace the old value with the appropriate bit length.
			mPixelData[i] = (VInt32)round((VFloat)pixel * convertFactor);
		}

		// Once bit conversion complete, forget old value.
		this->mBitLength = targetBitLength;
	}
}

VInt32 &DicomImage::Get(VUInt32 row, VUInt32 column, VUInt32 channel) const
{
	return mPixelData[(column * GetColumnCount() + row) * GetChannelCount() + channel];

	/*VUInt32 pixelIndex = 0;
	VInt32 *pixel;
	bool pixelFound = false;

	for (VUInt32 rowIndex = 0; rowIndex < GetRowCount(); rowIndex++)
	{
		for (VUInt32 columnIndex = 0; columnIndex < GetColumnCount(); columnIndex++)
		{
			for (VUInt32 channelIndex = 0; channelIndex < GetChannelCount(); channelIndex++)
			{
				if ((rowIndex == row) &&
					(columnIndex == column) &&
					(channelIndex == channel))
				{
					pixel = &mPixels[pixelIndex];
					pixelFound = true;
				}

				pixelIndex++;
			}
		}
	}

	if (!pixelFound)
	{
		THROW_VIMRID_EX(
			"Could not find pixel at " << row << "x" << column << "x" << channel << ".");
	}

	return *pixel;*/
}

ImageMatrix *DicomImage::ToImageMatrix()
{
	ImageMatrix *matrix =
		new ImageMatrix(
			GetRowCount(),
			GetColumnCount(),
			GetChannelCount());

	VInt32 *pixels = CopyPixelsToInt32();
	VUInt32 pixelIndex = 0;

	for (VUInt32 rowIndex = 0; rowIndex < GetRowCount(); rowIndex++)
	{
		for (VUInt32 columnIndex = 0; columnIndex < GetColumnCount(); columnIndex++)
		{
			for (VUInt32 channelIndex = 0; channelIndex < GetChannelCount(); channelIndex++)
			{
				//matrix[rowIndex][columnIndex][channelIndex] = pixels[pixelIndex++];
				matrix->Get(rowIndex, columnIndex, channelIndex) = pixels[pixelIndex++];
			}
		}
	}

	return matrix;
}

void DicomImage::UpdateFromImageMatrix(ImageMatrix matrix)
{
	if (matrix.GetRowCount() != GetRowCount())
	{
		THROW_VIMRID_EX(
			"Matrix row count (" << matrix.GetRowCount() <<
			") does not match that of the DICOM image (" <<
			GetRowCount() << ").");
	}

	if (matrix.GetColumnCount() != GetColumnCount())
	{
		THROW_VIMRID_EX(
			"Matrix column count (" << matrix.GetColumnCount() <<
			") does not match that of the DICOM image (" <<
			GetColumnCount() << ").");
	}

	if (matrix.GetChannelCount() != GetChannelCount())
	{
		THROW_VIMRID_EX(
			"Matrix channel count (" << matrix.GetChannelCount() <<
			") does not match that of the DICOM image (" <<
			GetChannelCount() << ").");
	}

	VUInt32 pixelIndex = 0;
	for (VUInt32 rowIndex = 0; rowIndex < GetRowCount(); rowIndex++)
	{
		for (VUInt32 columnIndex = 0; columnIndex < GetColumnCount(); columnIndex++)
		{
			for (VUInt32 channelIndex = 0; channelIndex < GetChannelCount(); channelIndex++)
			{
				//mPixels[pixelIndex++] = matrix[rowIndex][columnIndex][channelIndex];
				mPixelData[pixelIndex++] = matrix.Get(rowIndex, columnIndex, channelIndex);
			}
		}
	}
}

}
}
