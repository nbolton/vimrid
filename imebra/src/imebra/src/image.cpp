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

/*! \file image.cpp
    \brief Implementation of the class image.

*/

#include "../../base/include/exception.h"
#include "../include/image.h"
#include "../include/colorTransformsFactory.h"

namespace puntoexe
{

namespace imebra
{

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
//
// image
//
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Create an image with the specified size, colorspace and
//  bit depth
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
ptr<handlers::imageHandler> image::create(
						const imbxUint32 sizeX, 
						const imbxUint32 sizeY, 
						const bitDepth depth, 
						std::wstring inputColorSpace, 
						const imbxUint8 highBit)
{
	PUNTOEXE_FUNCTION_START(L"image::create");

	lockObject lockAccess(this);

	if(sizeX == 0 || sizeY == 0)
	{
		PUNTOEXE_THROW(imageExceptionInvalidSize, "An invalid image's size has been specified");
	}

	// Normalize the color space (remove _420 & _422 and
	//  make it uppercase).
	///////////////////////////////////////////////////////////
	m_colorSpace=transforms::colorTransforms::colorTransformsFactory::normalizeColorSpace(inputColorSpace);

	// Find the number of channels to allocate
	///////////////////////////////////////////////////////////
	m_channelsNumber = transforms::colorTransforms::colorTransformsFactory::getNumberOfChannels(inputColorSpace);
	if(m_channelsNumber == 0)
	{
		PUNTOEXE_THROW(imageExceptionUnknownColorSpace, "Cannot recognize the specified color space");
	}

	// Find the datatype to use to allocate the
	//  buffer (datatypes are in Dicom standard, plus SB
	//  for signed bytes).
	///////////////////////////////////////////////////////////
	m_channelPixelSize = 0;
	imbxUint8 defaultHighBit = 0;

	switch(depth)
	{
	case depthU8:
		defaultHighBit=7;
		break;
	case depthS8:
		defaultHighBit=7;
		break;
	case depthU16:
		defaultHighBit=15;
		break;
	case depthS16:
		defaultHighBit=15;
		break;
	case depthUnknown:
		defaultHighBit=0;
		break;
	default:
		PUNTOEXE_THROW(imageExceptionUnknownDepth, "Unknown depth");
	}

	// Adjust the high bit value
	///////////////////////////////////////////////////////////
	if(highBit == 0 || highBit>defaultHighBit)
		m_highBit=defaultHighBit;
	else
		m_highBit=highBit;

	// If a valid buffer with the same data type is already
	//  allocated then use it.
	///////////////////////////////////////////////////////////
	if(m_buffer == 0 || !(m_buffer->isReferencedOnce()) )
	{
		ptr<buffer> tempBuffer(new buffer(m_externalLock, "SL"));
		m_buffer = tempBuffer;
	}

	m_sizeX = m_sizeY = 0;
	
	ptr<handlers::dataHandler> imageHandler = m_buffer->getDataHandler(true, sizeX * sizeY * (imbxUint32)m_channelsNumber);
	if(imageHandler != 0)
	{
		m_rowLength = m_channelsNumber*sizeX;
		
		imageHandler->setSize(m_rowLength*sizeY);
		m_channelPixelSize = imageHandler->getUnitSize();

		// Set the attributes
		///////////////////////////////////////////////////////////
		m_imageDepth=depth;
		m_sizeX=sizeX;
		m_sizeY=sizeY;
	}

	return ptr<handlers::imageHandler>(dynamic_cast<handlers::imageHandler*>(imageHandler.get()) );

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set the depth
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void image::setBitDepthAndHighBit(imbxUint32 highBit, bitDepth imageBitDepth)
{
	PUNTOEXE_FUNCTION_START(L"image::setBitDepthAndHighBit");

	lockObject lockAccess(this);

	if(highBit != 0)
	{
		m_highBit = highBit;
	}

	if(imageBitDepth != depthUnknown)
	{
		m_imageDepth = imageBitDepth;
	}

	// If the bit depth is unknown, then calculate it
	///////////////////////////////////////////////////////////
	if(m_imageDepth != depthUnknown && m_imageDepth != 0)
	{
		return;
	}

	// Find the requested bit depth
	///////////////////////////////////////////////////////////
	bitDepth finalBitDepth = image::depthU8;
	imbxUint32 finalHighBit = 7;

	imbxUint32 rowSize, pixelSize, channelsNumber;
	ptr<handlers::imageHandler> imageDataHandler = getDataHandler(false, &rowSize, &pixelSize, &channelsNumber);
	imbxInt32* pBuffer = imageDataHandler->getMemoryBuffer();

	imbxInt32 minimumValue=*pBuffer;
	imbxInt32 maximumValue=*pBuffer;
	imbxInt32 tempValue;
	for(imbxUint32 bufferSize = m_sizeX * m_sizeY * channelsNumber; bufferSize != 0; --bufferSize)
	{
		tempValue=*(pBuffer++);
		if(tempValue<minimumValue)
			minimumValue=tempValue;
		if(tempValue>maximumValue)
			maximumValue=tempValue;
	}

	// Signed bit depth
	///////////////////////////////////////////////////////////
	if(minimumValue < 0)
	{
		finalBitDepth = depthS8;
		if(minimumValue < -128L || maximumValue >= 128L)
		{
			finalBitDepth = depthS16;
			finalHighBit = 15;
		}
	}
	
	// Unsigned bit depth
	///////////////////////////////////////////////////////////
	else if(maximumValue >= 256L)
	{
		finalBitDepth = depthU16;
		finalHighBit = 15;
	}

	if(m_highBit > 7)
	{
		if(finalBitDepth == depthU8)
		{
			finalBitDepth = depthU16;
		}
		if(finalBitDepth == depthS8)
		{
			finalBitDepth = depthS16;
		}
	}

	if(m_imageDepth == depthUnknown)
	{
		m_imageDepth = finalBitDepth;
	}

	if(m_highBit == 0)
	{
		m_highBit = finalHighBit;
	}

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Retrieve an handler to the image's buffer.
// The image's components are interleaved.
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
ptr<handlers::imageHandler> image::getDataHandler(const bool bWrite, imbxUint32* pRowSize, imbxUint32* pChannelPixelSize, imbxUint32* pChannelsNumber)
{
	PUNTOEXE_FUNCTION_START(L"image::getDataHandler");

	lockObject lockAccess(this);

	if(m_buffer == 0)
	{
		return ptr<handlers::imageHandler>(0);
	}

	*pRowSize=m_rowLength;
	*pChannelPixelSize=m_channelPixelSize;
	*pChannelsNumber=m_channelsNumber;

	ptr<handlers::dataHandler> ptrDataHandler = m_buffer->getDataHandler(bWrite, m_sizeX * m_sizeY * m_channelsNumber);
	handlers::imageHandler* pDataHandlerImage = dynamic_cast<handlers::imageHandler*> (ptrDataHandler.get());
	ptr<handlers::imageHandler> ptrDataHandlerImage(pDataHandlerImage);

	return ptrDataHandlerImage;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Return the bit depth
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
image::bitDepth image::getDepth()
{
	PUNTOEXE_FUNCTION_START(L"image::getDepth");

	lockObject lockAccess(this);
	return m_imageDepth;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Return the high bit
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxUint32 image::getHighBit()
{
	PUNTOEXE_FUNCTION_START(L"image::getHighBit");

	lockObject lockAccess(this);
	return m_highBit;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Returns the colorspace
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::wstring image::getColorSpace()
{
	PUNTOEXE_FUNCTION_START(L"image::getColorSpace");

	lockObject lockAccess(this);
	return m_colorSpace;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Returns the numbero of allocated channels
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxUint32 image::getChannelsNumber()
{
	PUNTOEXE_FUNCTION_START(L"image::getChannelsNumber");

	lockObject lockAccess(this);
	return m_channelsNumber;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Returns the image's size in pixels
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void image::getSize(imbxUint32* pSizeX, imbxUint32* pSizeY)
{
	PUNTOEXE_FUNCTION_START(L"image::getSize");

	lockObject lockAccess(this);

	if(pSizeX)
		*pSizeX=m_sizeX;

	if(pSizeY)
		*pSizeY=m_sizeY;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Returns the image's size in millimeters
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void image::getSizeMm(double* pSizeMmX, double* pSizeMmY)
{
	PUNTOEXE_FUNCTION_START(L"image::getSizeMm");

	lockObject lockAccess(this);

	if(pSizeMmX)
		*pSizeMmX=m_sizeMmX;

	if(pSizeMmY)
		*pSizeMmY=m_sizeMmY;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set the image's size in millimeters
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void image::setSizeMm(const double sizeMmX, const double sizeMmY)
{
	PUNTOEXE_FUNCTION_START(L"image::setSizeMm");

	lockObject lockAccess(this);

	if(sizeMmX)
		m_sizeMmX=sizeMmX;

	if(sizeMmY)
		m_sizeMmY=sizeMmY;

	PUNTOEXE_FUNCTION_END();
}

} // namespace imebra

} // namespace puntoexe
