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

/*! \file drawBitmap.cpp
    \brief Implementation of the transform drawBitmap.

*/

#include "../include/drawBitmap.h"
#include "../include/image.h"
#include "../include/colorTransformsFactory.h"
#include "../include/transformHighBit.h"
#include <string.h>

namespace puntoexe
{

namespace imebra
{

namespace transforms
{

drawBitmap::drawBitmap():
	m_totalWidthPixels(0), m_totalHeightPixels(0),
	m_visibleTopLeftX(0), m_visibleTopLeftY(0),	m_visibleBottomRightX(0), m_visibleBottomRightY(0),
	m_destBitmapWidth(0), m_destBitmapHeight(0),
	m_destBitmapRowSize(0),
	m_alignByte(1),	m_bBGR(false)
{
}


///////////////////////////////////////////////////////////
//
// Declare the input image
//
///////////////////////////////////////////////////////////
void drawBitmap::declareInputImage(long imageNumber, ptr<image> pInputImage)
{
	PUNTOEXE_FUNCTION_START(L"");

	// If the dataset hasn't been declared then throw an
	//  exception
	///////////////////////////////////////////////////////////
	if(getDataSet() == 0)
	{
		PUNTOEXE_THROW(drawBitmapExceptionDataSetNotDeclared, "The dataSet hasn't been declared");
	}

	// Create an output image if it isn't available
	///////////////////////////////////////////////////////////
	ptr<image> pFinalImage(getInputImage(imageNumber));
	if(pFinalImage == 0)
	{
		pFinalImage = new image;
	}

	// Create a temporary image if it isn't available
	///////////////////////////////////////////////////////////
	if(m_pTemporaryImage == 0)
	{
		m_pTemporaryImage = new image;
	}

	// Get the color transform
	///////////////////////////////////////////////////////////
	ptr<transforms::colorTransforms::colorTransformsFactory> pColorFactory =
		transforms::colorTransforms::colorTransformsFactory::getColorTransformsFactory();
	ptr<transforms::transform> rgbTransform = pColorFactory->getTransform(pInputImage->getColorSpace(), L"RGB");

	// Get the high bit transform
	///////////////////////////////////////////////////////////
	ptr<transforms::transformHighBit> highBit(new transforms::transformHighBit);

	// Execute the color transform and set the input for the
	//  high bit transform
	///////////////////////////////////////////////////////////
	if(rgbTransform != 0)
	{
		rgbTransform->declareDataSet(getDataSet());
		rgbTransform->declareInputImage(0, pInputImage);
		rgbTransform->declareOutputImage(0, m_pTemporaryImage);
		rgbTransform->doTransform();
		highBit->declareInputImage(0, m_pTemporaryImage);
	}
	else
	{
		highBit->declareInputImage(0, pInputImage);
	}

	// Convert to 8 bits
	///////////////////////////////////////////////////////////
	imbxUint32 sizeX, sizeY;
	pInputImage->getSize(&sizeX, &sizeY);
	pFinalImage->create(sizeX, sizeY, image::depthU8, L"RGB", 7);
	highBit->declareOutputImage(0, pFinalImage);
	highBit->doTransform();

	transform::declareInputImage(imageNumber, pFinalImage);

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
//
// Setup the portion of the bitmap that has to be retrieved
//
///////////////////////////////////////////////////////////
void drawBitmap::declareBitmapType(imbxInt32 totalWidthPixels, imbxInt32 totalHeightPixels,
		imbxInt32 visibleTopLeftX, imbxInt32 visibleTopLeftY, imbxInt32 visibleBottomRightX, imbxInt32 visibleBottomRightY,
		imbxInt32 alignByte,
		bool bBGR)
{
	PUNTOEXE_FUNCTION_START(L"drawBitmap::declareBitmapType");

	if(m_alignByte == 0)
	{
		PUNTOEXE_THROW(drawBitmapExceptionInvalidAlignByte, "Align byte not valid");
	}

	m_totalWidthPixels = totalWidthPixels;
	m_totalHeightPixels = totalHeightPixels;
	m_visibleTopLeftX = visibleTopLeftX;
	m_visibleTopLeftY = visibleTopLeftY;
	m_visibleBottomRightX = visibleBottomRightX;
	m_visibleBottomRightY = visibleBottomRightY;
	m_alignByte = alignByte;
	m_bBGR = bBGR;

	// Check if the image is visible in the specified area
	///////////////////////////////////////////////////////////
	if(
		m_visibleBottomRightX > m_totalWidthPixels ||
		m_visibleBottomRightY > m_totalHeightPixels ||
		m_visibleTopLeftX < 0 ||
		m_visibleTopLeftY < 0 ||
		m_visibleTopLeftX > m_visibleBottomRightX ||
		m_visibleTopLeftY > m_visibleBottomRightY
		)
	{
		PUNTOEXE_THROW(drawBitmapExceptionInvalidArea, "Destination area not valid");
	}

	if(m_destBitmapWidth != m_visibleBottomRightX - m_visibleTopLeftX ||
		m_destBitmapHeight != m_visibleBottomRightY - m_visibleTopLeftY)
	{
		m_destBitmapRowSize = ((m_visibleBottomRightX - m_visibleTopLeftX) * 3 + m_alignByte - 1) / m_alignByte;
		m_destBitmapRowSize *= m_alignByte;

		m_finalBitmap = memoryPool::getMemoryPool()->getMemory(m_destBitmapRowSize * (m_visibleBottomRightY - m_visibleTopLeftY));
		m_destBitmapHeight = m_visibleBottomRightY - m_visibleTopLeftY;

	}

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
//
// Retrieve the bitmap
//
///////////////////////////////////////////////////////////
imbxUint8* drawBitmap::getOutputBitmap(imbxInt32* pWidthPixels, imbxInt32* pHeightPixels, imbxInt32* pRowLengthBytes)
{
	*pWidthPixels = m_destBitmapWidth;
	*pHeightPixels = m_destBitmapHeight;
	*pRowLengthBytes = m_destBitmapRowSize;
	return (imbxUint8*)(m_finalBitmap->data());
}


///////////////////////////////////////////////////////////
//
// Draw the window's content
//
///////////////////////////////////////////////////////////
void drawBitmap::doTransform()
{
	PUNTOEXE_FUNCTION_START(L"drawBitmap::doTransform");

	ptr<image> inputImage(getInputImage(0));

	if(inputImage == 0)
	{
		PUNTOEXE_THROW(drawBitmapExceptionImageNotDeclared, "Input image not declared");
	}

	if( m_visibleTopLeftX == m_visibleBottomRightX ||
		m_visibleTopLeftY == m_visibleBottomRightY)
	{
		return;
	}



	// Retrieve the image's buffer
	///////////////////////////////////////////////////////////
	imbxUint32 rowSize, channelSize, channelsNumber;
	ptr<handlers::imageHandler> handler = inputImage->getDataHandler(false, &rowSize, &channelSize, &channelsNumber);
	imbxInt32* imageMemory = handler->getMemoryBuffer();

	// Find the multiplier that make the image bigger than
	//  the rendering area
	///////////////////////////////////////////////////////////
	imbxUint32 imageSizeX, imageSizeY;
	inputImage->getSize(&imageSizeX, &imageSizeY);

	imbxUint8 leftShiftX(0);
	imbxUint32 maskX(0);
	while( (imageSizeX << leftShiftX) < (imbxUint32)m_totalWidthPixels)
	{
		++leftShiftX;
		maskX <<= 1;
		++maskX;
	}

	imbxUint8 leftShiftY(0);
	while( (imageSizeY << leftShiftY) < (imbxUint32)m_totalHeightPixels)
	{
		++leftShiftY;
	}

	// Allocate an horizontal buffer that stores the pixels
	//  average colors
	///////////////////////////////////////////////////////////
	if(m_destBitmapWidth != m_visibleBottomRightX - m_visibleTopLeftX)
	{
		m_averagePixels.reset(new imbxInt32[(m_visibleBottomRightX - m_visibleTopLeftX) << 2]);
		m_sourcePixelIndex.reset(new imbxUint32[m_visibleBottomRightX - m_visibleTopLeftX + 1]);
		m_destBitmapWidth = m_visibleBottomRightX - m_visibleTopLeftX;
	}

	for(imbxInt32 scanPixelsX = m_visibleTopLeftX; scanPixelsX != m_visibleBottomRightX + 1; ++scanPixelsX)
	{
		m_sourcePixelIndex.get()[scanPixelsX - m_visibleTopLeftX] = scanPixelsX * (imageSizeX << leftShiftX) / m_totalWidthPixels;
	}

	imbxUint8* pFinalBuffer = (imbxUint8*)(m_finalBitmap->data());
	imbxInt32 nextRowGap = m_destBitmapRowSize - m_destBitmapWidth * 3;

	for(imbxInt32 scanY = m_visibleTopLeftY; scanY != m_visibleBottomRightY; ++scanY)
	{
		::memset(m_averagePixels.get(), 0, (m_destBitmapWidth << 2) * sizeof(m_averagePixels.get()[0]));

		imbxInt32 firstPixelY = scanY * (imageSizeY << leftShiftY) / m_totalHeightPixels;
		imbxInt32 lastPixelY = (scanY + 1) * (imageSizeY << leftShiftY) / m_totalHeightPixels;
		for(imbxInt32 scanImageY = firstPixelY; scanImageY != lastPixelY; ++ scanImageY)
		{
			imbxInt32* pAveragePointer = m_averagePixels.get();
			imbxUint32* pNextSourceXIndex = m_sourcePixelIndex.get();

			imbxInt32* pImagePointer = &(imageMemory[(scanImageY >> leftShiftY) * imageSizeX * channelsNumber + ((*pNextSourceXIndex) >> leftShiftX) * channelsNumber]);

			for(imbxInt32 scanX = m_visibleTopLeftX; scanX != m_visibleBottomRightX; ++scanX)
			{
				for(imbxUint32 scanImageX = *(pNextSourceXIndex++); scanImageX != *pNextSourceXIndex; ++scanImageX)
				{
					++(*(pAveragePointer++));
					*(pAveragePointer++) += *(pImagePointer++);
					*(pAveragePointer++) += *(pImagePointer++);
					*(pAveragePointer++) += *(pImagePointer++);
					pAveragePointer -= 4;
					if( (scanImageX & maskX) == 0)
					{
						continue;
					}
					pImagePointer -= 3;
				}
				pAveragePointer += 4;
			}
		}

		// Copy the average to the bitmap
		imbxInt32* pAveragePointer = m_averagePixels.get();
		imbxUint32 counter;

		if(m_bBGR)
		{
			imbxUint32 r, g;
			for(imbxInt32 scanX = m_visibleTopLeftX; scanX != m_visibleBottomRightX; ++scanX)
			{
				counter = (imbxUint32)*(pAveragePointer++);
				r = (imbxUint8) (((imbxUint32)*(pAveragePointer++) / counter) & 0xff);
				g = (imbxUint8) (((imbxUint32)*(pAveragePointer++) / counter) & 0xff);
				*(pFinalBuffer++) = (imbxUint8) (((imbxUint32)*(pAveragePointer++) / counter) & 0xff);
				*(pFinalBuffer++) = (imbxUint8)g;
				*(pFinalBuffer++) = (imbxUint8)r;
			}
		}
		else
		{
			for(imbxInt32 scanX = m_visibleTopLeftX; scanX != m_visibleBottomRightX; ++scanX)
			{
				counter = (imbxUint32)*(pAveragePointer++);
				*(pFinalBuffer++) = (imbxUint8) (((imbxUint32)*(pAveragePointer++) / counter) & 0xff);
				*(pFinalBuffer++) = (imbxUint8) (((imbxUint32)*(pAveragePointer++) / counter) & 0xff);
				*(pFinalBuffer++) = (imbxUint8) (((imbxUint32)*(pAveragePointer++) / counter) & 0xff);
			}
		}
		pFinalBuffer += nextRowGap;
	}

	PUNTOEXE_FUNCTION_END();
}

} // namespace transforms

} // namespace imebra

} // namespace puntoexe
