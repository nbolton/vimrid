/*

0.0.46

Imebra: a C++ dicom library.
Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008  by Paolo Brandoli

This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU AFFERO GENERAL PUBLIC LICENSE Version 3 for more details.

You should have received a copy of the GNU AFFERO GENERAL PUBLIC LICENSE Version 3
 along with this program; If not, see http://www.gnu.org/licenses/

-------------------

If you want to use Imebra commercially then you have to buy the commercial
 license available at http://puntoexe.com
 
After you buy the commercial license then you can use Imebra according
 to the terms described in the Imebra Commercial License Version 1.
A copy of the Imebra Commercial License Version 1 is available in the 
 documentation pages.

Imebra is available at http://puntoexe.com

The author can be contacted by email at paolo@puntoexe.com or by mail at
 the following address:
 Paolo Brandoli
 Preglov trg 6
 1000 Ljubljana
 Slovenia


*/

/*! \file transformBuffers.cpp
    \brief Implementation of the class transformBuffers.

*/

#include "../../base/include/exception.h"
#include "../include/transformBuffers.h"
#include <string.h>


namespace puntoexe
{

namespace imebra
{

namespace transforms
{


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Process the images
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void transformBuffers::doTransform()
{
	PUNTOEXE_FUNCTION_START(L"transformBuffers::doTransform");

	// Process all the input images
	///////////////////////////////////////////////////////////
	for(int scanImages = 0; ; ++scanImages)
	{
		// Get the input image
		///////////////////////////////////////////////////////////
		ptr<image> inputImage = getInputImage(scanImages);

		// Get the output image
		///////////////////////////////////////////////////////////
		ptr<image> outputImage = getOutputImage(scanImages);

		// If both the input and the output images are empty,
		//  then exit
		///////////////////////////////////////////////////////////
		if(inputImage == 0 && outputImage == 0)
		{
			break;
		}

		// If the input image is empty, then use the output
		//  image as input/output
		///////////////////////////////////////////////////////////
		if(inputImage == 0)
		{
			inputImage = outputImage;
		}
		imbxUint32 inputSizeX, inputSizeY;
		double sizeMmX, sizeMmY;
		inputImage->getSize(&inputSizeX, &inputSizeY);
		inputImage->getSizeMm(&sizeMmX, &sizeMmY);
		image::bitDepth inputBitDepth = inputImage->getDepth();
		imbxUint32 inputHighBit = inputImage->getHighBit();

		image::bitDepth outputBitDepth = inputBitDepth;
		imbxUint32 outputHighBit = inputHighBit;

		if(outputImage == 0)
		{
			outputImage = ptr<image>(new image);
			declareOutputImage(scanImages, outputImage);
		}
		else
		{
			if(outputImage->getDepth() != image::depthUnknown)
			{
				outputBitDepth = outputImage->getDepth();
				outputHighBit = outputImage->getHighBit();
			}
		}

		std::wstring inputColorSpace = inputImage->getColorSpace();
		if(inputImage != outputImage)
		{
			outputImage->create(inputSizeX, inputSizeY, image::depthUnknown, inputColorSpace, 0);
			outputImage->setSizeMm(sizeMmX, sizeMmY);
		}

		processDataBuffers(inputImage, outputImage,
                        inputSizeX, inputSizeY,
                        inputColorSpace, inputBitDepth, inputHighBit,
                        &outputBitDepth, &outputHighBit);

        // If the bit depth is unknown, then calculate it
        ///////////////////////////////////////////////////////////
        outputImage->setBitDepthAndHighBit(outputHighBit, outputBitDepth);
	}

	PUNTOEXE_FUNCTION_END();
}


void transformBuffersInputOutput::processDataBuffers(ptr<image> inputImage, ptr<image> outputImage,
		imbxUint32 sizeX,
		imbxUint32 sizeY,
		std::wstring inputColorSpace,
		image::bitDepth inputDepth,
		imbxUint32 inputHighBit,
		image::bitDepth* pOutputDepth,
		imbxUint32* pOutputHighBit)

{
    PUNTOEXE_FUNCTION_START(L"transformBuffers::processDataBuffers");

    // Get the input image's attributes and the data handler
    ///////////////////////////////////////////////////////////
    imbxUint32 rowSize, pixelSize, channelsNumber;

    ptr<handlers::imageHandler> inputDataHandler=inputImage->getDataHandler(false, &rowSize, &pixelSize, &channelsNumber);
    ptr<handlers::imageHandler> outputDataHandler=outputImage->getDataHandler(true, &rowSize, &pixelSize, &channelsNumber);

    // Process the buffers
    ///////////////////////////////////////////////////////////
    imbxInt32* pInputMemory = inputDataHandler->getMemoryBuffer();
    imbxInt32* pOutputMemory = outputDataHandler->getMemoryBuffer();

    // Calculate the buffer's size
    ///////////////////////////////////////////////////////////
    imbxUint32 inputBufferSize = sizeX * sizeY * channelsNumber;

    doTransformBuffersInputOutput(
        sizeX,
        sizeY,
        channelsNumber,
        inputColorSpace,
        inputDepth,
        inputHighBit,
        pInputMemory,
        pOutputMemory,
        inputBufferSize,
        pOutputDepth,
        pOutputHighBit
        );

    PUNTOEXE_FUNCTION_END();
}


void transformBuffersInPlace::processDataBuffers(ptr<image> inputImage, ptr<image> outputImage,
		imbxUint32 sizeX,
		imbxUint32 sizeY,
		std::wstring inputColorSpace,
		image::bitDepth inputDepth,
		imbxUint32 inputHighBit,
		image::bitDepth* pOutputDepth,
		imbxUint32* pOutputHighBit)
{
    PUNTOEXE_FUNCTION_START(L"transformBuffersInPlace::processDataBuffers");

    // Get the input image's attributes and the data handler
    ///////////////////////////////////////////////////////////
    imbxUint32 rowSize, pixelSize, channelsNumber;

    ptr<handlers::imageHandler> outputDataHandler=outputImage->getDataHandler(true, &rowSize, &pixelSize, &channelsNumber);
    imbxInt32* pOutputMemory = outputDataHandler->getMemoryBuffer();
    if(inputImage != outputImage)
    {
        ptr<handlers::imageHandler> inputDataHandler=inputImage->getDataHandler(false, &rowSize, &pixelSize, &channelsNumber);
        imbxInt32* pInputMemory = inputDataHandler->getMemoryBuffer();
        ::memcpy(pOutputMemory, pInputMemory, sizeX * sizeY * channelsNumber * sizeof(imbxInt32));
    }

    // Calculate the buffer's size
    ///////////////////////////////////////////////////////////
    imbxUint32 inputBufferSize = sizeX * sizeY * channelsNumber;

    // Process the buffers
    ///////////////////////////////////////////////////////////
    doTransformBuffersInPlace(
        sizeX,
        sizeY,
        channelsNumber,
        inputColorSpace,
        inputDepth,
        inputHighBit,
        pOutputMemory,
        inputBufferSize,
        pOutputDepth,
        pOutputHighBit
        );

    PUNTOEXE_FUNCTION_END();

}

} // namespace transforms

} // namespace imebra

} // namespace puntoexe
