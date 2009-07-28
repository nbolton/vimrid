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

/*! \file MONOCHROME1ToMONOCHROME2.cpp
    \brief Implementation of the classes for conversion between 2 monochrome color spaces.

*/

#include "../../base/include/exception.h"
#include "../include/MONOCHROME1ToMONOCHROME2.h"
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

static registerColorTransform m_registerTransform0(ptr<colorTransform>(new MONOCHROME1ToMONOCHROME2));
static registerColorTransform m_registerTransform1(ptr<colorTransform>(new MONOCHROME2ToMONOCHROME1));

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Return the initial color space
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::wstring MONOCHROME1ToMONOCHROME2::getInitialColorSpace()
{
	return L"MONOCHROME1";
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
std::wstring MONOCHROME1ToMONOCHROME2::getFinalColorSpace()
{
	return L"MONOCHROME2";
}

ptr<colorTransform> MONOCHROME1ToMONOCHROME2::createColorTransform()
{
	return ptr<colorTransform> (new MONOCHROME1ToMONOCHROME2);
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// MONOCHROME1 to MONOCHROME2 transformation
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void MONOCHROME1ToMONOCHROME2::doColorTransform(imbxInt32* pSourceMem, imbxInt32* pDestMem, imbxUint32 pixelsNumber, imbxInt32 /* inputMinValue */, imbxInt32 inputMaxValue, imbxInt32 outputMinValue, imbxInt32 outputMaxValue)
{
	imbxInt32 destPixel;
	imbxInt32 offset=inputMaxValue+outputMinValue;
	while(pixelsNumber--)
	{
		///////////////////////////////////////////////////////////
		// Conversion
		///////////////////////////////////////////////////////////
		destPixel=offset - *pSourceMem++;

		if(destPixel<outputMinValue)
			destPixel=outputMinValue;
		if(destPixel>outputMaxValue)
			destPixel=outputMaxValue;

		*pDestMem++ = destPixel;
	}
}



///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Return the initial color space
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::wstring MONOCHROME2ToMONOCHROME1::getInitialColorSpace()
{
	return L"MONOCHROME2";
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
std::wstring MONOCHROME2ToMONOCHROME1::getFinalColorSpace()
{
	return L"MONOCHROME1";
}

ptr<colorTransform> MONOCHROME2ToMONOCHROME1::createColorTransform()
{
	return ptr<colorTransform> (new MONOCHROME2ToMONOCHROME1);
}

} // namespace colorTransforms

} // namespace transforms

} // namespace imebra

} // namespace puntoexe

