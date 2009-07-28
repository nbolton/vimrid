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

/*! \file RGBToYBRFULL.cpp
    \brief Implementation of the class RGBToYBRFULL.

*/

#include "../../base/include/exception.h"
#include "../include/RGBToYBRFULL.h"
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

static registerColorTransform m_registerTransform(ptr<colorTransform>(new RGBToYBRFULL));

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Return the initial color space
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::wstring RGBToYBRFULL::getInitialColorSpace()
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
std::wstring RGBToYBRFULL::getFinalColorSpace()
{
	return L"YBR_FULL";
}


ptr<colorTransform> RGBToYBRFULL::createColorTransform()
{
	return ptr<colorTransform>(new RGBToYBRFULL);
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
void RGBToYBRFULL::doColorTransform(imbxInt32* pSourceMem, imbxInt32* pDestMem, imbxUint32 pixelsNumber, imbxInt32 /* inputMinValue */, imbxInt32 /* inputMaxValue */, imbxInt32 outputMinValue, imbxInt32 outputMaxValue)
{
	static const int precisionBits(14);
	static const double multiplier((double)((imbxInt32)1 << precisionBits));
	static const imbxInt32 multiplier0_5((imbxUint32)1 << (precisionBits - 1));
	static const imbxInt32 multiplier0_2990((imbxInt32)(0.2990f * multiplier + 0.5));
	static const imbxInt32 multiplier0_5870((imbxInt32)(0.5870f * multiplier + 0.5));
	static const imbxInt32 multiplier0_1140((imbxInt32)(0.1140f * multiplier + 0.5));
	static const imbxInt32 multiplier0_1687((imbxInt32)(0.1687f * multiplier + 0.5));
	static const imbxInt32 multiplier0_3313((imbxInt32)(0.3313f * multiplier + 0.5));
	static const imbxInt32 multiplier0_4187((imbxInt32)(0.4187f * multiplier + 0.5));
	static const imbxInt32 multiplier0_0813((imbxInt32)(0.0813f * multiplier + 0.5));

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
		destPixelY = (multiplier0_2990 * sourcePixelR + multiplier0_5870 * sourcePixelG + multiplier0_1140 * sourcePixelB + multiplier0_5) >> precisionBits;
		destPixelB = middleValue + ((multiplier0_5 * sourcePixelB - multiplier0_1687 * sourcePixelR - multiplier0_3313 * sourcePixelG + multiplier0_5) >> precisionBits);
		destPixelR = middleValue + ((multiplier0_5 * sourcePixelR - multiplier0_4187 * sourcePixelG - multiplier0_0813 * sourcePixelB + multiplier0_5) >> precisionBits);

		if(destPixelY<outputMinValue)
		{
			destPixelY=outputMinValue;
		}
		else if(destPixelY>outputMaxValue)
		{
			destPixelY=outputMaxValue;
		}

        if(destPixelB<outputMinValue)
		{
			destPixelB=outputMinValue;
		}
		else if(destPixelB>outputMaxValue)
		{
			destPixelB=outputMaxValue;
		}

		if(destPixelR<outputMinValue)
		{
			destPixelR=outputMinValue;
		}
		else if(destPixelR>outputMaxValue)
		{
			destPixelR=outputMaxValue;
		}

		*pDestMem++ = destPixelY;
		*pDestMem++ = destPixelB;
		*pDestMem++ = destPixelR;
	}
}

} // namespace colorTransforms

} // namespace transforms

} // namespace imebra

} // namespace puntoexe

