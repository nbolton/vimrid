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

/*! \file PALETTECOLORToRGB.cpp
    \brief Implementation of the class PALETTECOLORToRGB.

*/

#include "../../base/include/exception.h"
#include "../include/PALETTECOLORToRGB.h"
#include "../include/dataSet.h"
#include "../include/dataHandler.h"
#include "../include/image.h"

namespace puntoexe
{

namespace imebra
{

namespace transforms
{

namespace colorTransforms
{

static registerColorTransform m_registerTransform(ptr<colorTransform>(new PALETTECOLORToRGB));

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Return the initial color space
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::wstring PALETTECOLORToRGB::getInitialColorSpace()
{
	return L"PALETTE COLOR";
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Return the final color space
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::wstring PALETTECOLORToRGB::getFinalColorSpace()
{
	return L"RGB";
}

ptr<colorTransform> PALETTECOLORToRGB::createColorTransform()
{
	return ptr<colorTransform>(new PALETTECOLORToRGB);
}

void PALETTECOLORToRGB::declareDataSet(ptr<dataSet> pDataSet)
{
	PUNTOEXE_FUNCTION_START(L"PALETTECOLORToRGB::declareDataSet");

	lockObject lockAccess(this);
	colorTransform::declareDataSet(pDataSet);

	ptr<handlers::dataHandler> pRedDescriptor=pDataSet->getDataHandler(0x0028, 0x0, 0x1101, 0, false);
	ptr<handlers::dataHandler> pGreenDescriptor=pDataSet->getDataHandler(0x0028, 0x0, 0x1102, 0, false);
	ptr<handlers::dataHandler> pBlueDescriptor=pDataSet->getDataHandler(0x0028, 0x0, 0x1103, 0, false);
	ptr<handlers::dataHandler> pRedData=pDataSet->getDataHandler(0x0028, 0x0, 0x1201, 0, false);
	ptr<handlers::dataHandler> pGreenData=pDataSet->getDataHandler(0x0028, 0x0, 0x1202, 0, false);
	ptr<handlers::dataHandler> pBlueData=pDataSet->getDataHandler(0x0028, 0x0, 0x1203, 0, false);

	if(pRedDescriptor == 0 
		|| pGreenDescriptor == 0 
		|| pBlueDescriptor == 0 
		|| pRedData == 0 
		|| pGreenData == 0 
		|| pBlueData == 0)
	{
		return;
	}

	ptr<lut> redPalette(new lut);
	ptr<lut> greenPalette(new lut);
	ptr<lut> bluePalette(new lut);

	redPalette->setLut(pRedDescriptor, pRedData, L"");
	greenPalette->setLut(pGreenDescriptor, pGreenData, L"");
	bluePalette->setLut(pBlueDescriptor, pBlueData, L"");

	setLut(redPalette, greenPalette, bluePalette);

	PUNTOEXE_FUNCTION_END();
}

void PALETTECOLORToRGB::setLut(ptr<lut> red, ptr<lut> green, ptr<lut> blue)
{
	m_redPalette   = red;
	m_greenPalette = green;
	m_bluePalette  = blue;
}

void PALETTECOLORToRGB::doTransform()
{
	PUNTOEXE_FUNCTION_START(L"PALETTECOLORToRGB::doTransform");

	// Get the source image's palette
	///////////////////////////////////////////////////////////
	lut* pRed = m_redPalette.get();
	lut* pGreen = m_greenPalette.get();
	lut* pBlue = m_bluePalette.get();

	if(pRed == 0 || pGreen == 0 || pBlue == 0)
	{
		PUNTOEXE_THROW(transformExceptionDataSetNotDefined, "The lut or the dataset haven't been defined");
	}


	// Process all the input images
	///////////////////////////////////////////////////////////
	for(int scanImages = 0; ; ++scanImages)
	{
		// Get the input image
		///////////////////////////////////////////////////////////
		ptr<image> pInputImage=getInputImage(scanImages);

		// If the input image doesn't exist, then exit
		///////////////////////////////////////////////////////////
		if(pInputImage == 0)
			break;

		// Check the input color space
		///////////////////////////////////////////////////////////
		if(pInputImage->getColorSpace()!=getInitialColorSpace())
			break;

		// Get the output image
		///////////////////////////////////////////////////////////
		ptr<image> pOutputImage=getOutputImage(scanImages);
		if(pOutputImage == 0)
		{
			ptr<image> tempImage(new image);
			pOutputImage=tempImage;
			declareOutputImage(scanImages, pOutputImage);
		}

		// Get the input image's attributes and the data handler
		///////////////////////////////////////////////////////////
		imbxUint32 sizeX, sizeY;
		pInputImage->getSize(&sizeX, &sizeY);

		double sizeMmX, sizeMmY;
		pInputImage->getSizeMm(&sizeMmX, &sizeMmY);
		pOutputImage->setSizeMm(sizeMmX, sizeMmY);

		imbxUint8 paletteBits = pRed->getBits();
		image::bitDepth depth = image::depthU8;
		if(paletteBits >8)
		{
			depth = image::depthU16;
		}


		// Get the data handler for the input image
		///////////////////////////////////////////////////////////
		imbxUint32 rowSize, channelPixelSize, channelsNumber;
		ptr<handlers::imageHandler> pInputDataHandler = pInputImage->getDataHandler(false, &rowSize, &channelPixelSize, &channelsNumber);
		imbxInt32* pSourceBuffer = pInputDataHandler->getMemoryBuffer();

		// Create the output image
		///////////////////////////////////////////////////////////
		ptr<handlers::imageHandler> pOutputDataHandler = pOutputImage->create(sizeX, sizeY, depth, getFinalColorSpace(), paletteBits - 1);
		imbxInt32* pDestBuffer = pOutputDataHandler->getMemoryBuffer();

		imbxUint32 totalPixelsNumber=sizeX*sizeY;
		while(totalPixelsNumber--)
		{
			*pDestBuffer++ = pRed->mappedValue(*pSourceBuffer);
			*pDestBuffer++ = pGreen->mappedValue(*pSourceBuffer);
			*pDestBuffer++ = pBlue->mappedValue(*pSourceBuffer++);
		}
	}
	
	return;

	PUNTOEXE_FUNCTION_END();
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// MONOCHROME2 to RGB transformation
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void PALETTECOLORToRGB::doColorTransform(imbxInt32* /* pSourceMem */, imbxInt32* /* pDestMem */, imbxUint32 /* pixelsNumber */, imbxInt32 /* inputMinValue */, imbxInt32 /* inputMaxValue */, imbxInt32 /* outputMinValue */, imbxInt32 /* outputMaxValue */)
{
	PUNTOEXE_FUNCTION_START(L"PALETTECOLORToRGB::doColorTransform");

	PUNTOEXE_THROW(PALETTECOLORToRGBExceptionWhoToldYouSo, "Don't call this function directly!");

	PUNTOEXE_FUNCTION_END();
}

} // namespace colorTransforms

} // namespace transforms

} // namespace imebra

} // namespace puntoexe


