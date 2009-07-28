#include "DicomReader.h"
#include <imebra.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;
using namespace puntoexe;
using namespace puntoexe::imebra;
using namespace imebra;
using namespace imebra::transforms;
using namespace imebra::codecs;
using namespace imebra::handlers;
using namespace vimrid::dicom;

namespace vimrid
{
namespace dicom
{
namespace server
{

DicomImage *DicomReader::LoadImage(string fileName)
{
	// Open a stream for the DICOM file so we can read it.
	ptr<stream> readStream = new stream();
	readStream->openFile(fileName, std::ios::in);

	// Create a reader so that we can access the data from the stream.
	ptr<streamReader> reader = new streamReader(readStream);
	ptr<dataSet> dataSet = codecFactory::getCodecFactory()->load(reader);

	// Read the first image from the data set (there could be several).
	ptr<image> firstImage = dataSet->getImage(0);

	// TODO: Find out what they mean by VOILUT - not sure what it's for.
	// Convert the image from the original data set into its own data set - WHY?
	ptr<transform> modalityVoiLut = new modalityVOILUT();
	modalityVoiLut->declareInputImage(0, firstImage);
	modalityVoiLut->declareDataSet(dataSet);
	modalityVoiLut->doTransform();
	ptr<image> modalityVoiLutImage = modalityVoiLut->getOutputImage(0);

	// TODO: What is the difference between VIOLUT and modality VIOLUT?
	// Create a new VoiLut image from the modality VoiLut image - WHY?
	ptr<transform> voiLut = new VOILUT();
	voiLut->declareInputImage(0, modalityVoiLutImage);
	voiLut->declareDataSet(dataSet);
	voiLut->doTransform();
	ptr<image> finalImage = voiLut->getOutputImage(0);

	// The data reader allows us to read pixel data directly from the image.
	imbxUint32 rowSize, channelPixelSize, channelCount;
	ptr<imageHandler> dataHandler = finalImage->getDataHandler(
			false, &rowSize, &channelPixelSize, &channelCount);

	// Retrieve the image's size in pixels, so we can walk rows and columns.
	imbxUint32 rowCount, columnCount;
	finalImage->getSize(&rowCount, &columnCount);

	// This logic also exists in the DicomImage class, consider refactoring.
	VUInt32 pixelCount = columnCount * rowCount * channelCount;

	// Use an array on the heap so that we don't loose memory once out of scope.
	VInt32 *pixelArray = new VInt32[pixelCount];

	// Store pixel at each element, starting with 0.
	VUInt32 pixelIndex = 0;

	/* Number of bits for each pixel. The high bit is the
	 * highest bit in the DICOM image. For ease of use, 1 is
	 * added to represent the bit length.
	 */
	VByte bitLength = finalImage->getHighBit() + 1;

	/* Now for the interesting bit! Here we can walk over each pixel
	 * in the current DICOM image. Simply, we iterate over each row (Y)
	 * and column (X), then within the pixel we cycle through each colour
	 * channel (in gray level there's only 1 channel of values 0 to 255).
	 *
	 * The data is stored in rows, within each row is a column, within
	 * each column is a channel.
	 */
	for (imbxUint32 scanRow = 0; scanRow < rowCount; ++scanRow)
	{
		for (imbxUint32 scanColumn = 0; scanColumn < columnCount; ++scanColumn)
		{
			for (imbxUint32 scanChannel = 0; scanChannel < channelCount; ++scanChannel)
			{
				// Copy values to array which will be passed to DicomImage object.
				pixelArray[pixelIndex++] = dataHandler->getSignedLongIncPointer();
			}
		}
	}

	// Return a new image object on the heap so it can be used out of scope.
	return new DicomImage(rowCount, columnCount, channelCount, bitLength, pixelArray);
}

}
}
}
