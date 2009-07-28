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

/*! \file RGBToYBRPARTIAL.cpp
    \brief Implementation of the class RGBToYBRPARTIAL.

*/

#include "../../base/include/exception.h"
#include "../include/RGBToYBRPARTIAL.h"
#include "../include/dataHandler.h"
#include "../include/dataSet.h"
#include "../include/image.h"

namespace puntoexe
{

namespace imebra
{

namespace transforms
{

namespace colorTransforms
{

static registerColorTransform m_registerTransform(ptr<colorTransform>(new RGBToYBRPARTIAL));


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Return the initial color space
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::wstring RGBToYBRPARTIAL::getInitialColorSpace()
{
	return L"RGB";
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
std::wstring RGBToYBRPARTIAL::getFinalColorSpace()
{
	return L"YBR_PARTIAL";
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Create a RGBToYBRPARTIAL object
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
ptr<colorTransform> RGBToYBRPARTIAL::createColorTransform()
{
	return ptr<colorTransform>(new RGBToYBRPARTIAL);
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// RGB to YBR_FULL transformation
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void RGBToYBRPARTIAL::doColorTransform(imbxInt32* pSourceMem, imbxInt32* pDestMem, imbxUint32 pixelsNumber, imbxInt32 /* inputMinValue */, imbxInt32 /* inputMaxValue */, imbxInt32 outputMinValue, imbxInt32 outputMaxValue)
{
	imbxInt32 sourcePixelR;
	imbxInt32 sourcePixelG;
	imbxInt32 sourcePixelB;
	imbxInt32 destPixelY;
	imbxInt32 destPixelB;
	imbxInt32 destPixelR;
	imbxInt32 middleValue=(outputMaxValue-outputMinValue+1)>>1;
	while(pixelsNumber--)
	{
		sourcePixelR=*pSourceMem++;
		sourcePixelG=*pSourceMem++;
		sourcePixelB=*pSourceMem++;

		///////////////////////////////////////////////////////////
		// Conversion
		///////////////////////////////////////////////////////////
		destPixelY=(middleValue>>3)+(((imbxInt32)1052*sourcePixelR+(imbxInt32)2065*sourcePixelG+(imbxInt32)401*sourcePixelB)>>12);
		destPixelB=middleValue+(((imbxInt32)1799*sourcePixelB-(imbxInt32)607*sourcePixelR-(imbxInt32)1192*sourcePixelG)>>12);
		destPixelR=middleValue+(((imbxInt32)1799*sourcePixelR-(imbxInt32)1507*sourcePixelG-(imbxInt32)292*sourcePixelB)>>12);

		if(destPixelY<outputMinValue)
			destPixelY=outputMinValue;
		if(destPixelY>outputMaxValue)
			destPixelY=outputMaxValue;
		if(destPixelB<outputMinValue)
			destPixelB=outputMinValue;
		if(destPixelB>outputMaxValue)
			destPixelB=outputMaxValue;
		if(destPixelR<outputMinValue)
			destPixelR=outputMinValue;
		if(destPixelR>outputMaxValue)
			destPixelR=outputMaxValue;

		*pDestMem++ = destPixelY;
		*pDestMem++ = destPixelB;
		*pDestMem++ = destPixelR;
	}
}

} // namespace colorTransforms

} // namespace transforms

} // namespace imebra

} // namespace puntoexe


