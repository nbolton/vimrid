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

/*! \file transformHighBit.cpp
    \brief Implementation of the class transformHighBit.

*/

#include "../../base/include/exception.h"
#include "../include/transformHighBit.h"
#include "../include/dataSet.h"


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
// Modify the high bit of an image
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void transformHighBit::doTransformBuffersInPlace(
	imbxUint32 /* sizeX */,
	imbxUint32 /* sizeY */,
	imbxUint32 /* inputChannelsNumber */,
	std::wstring /* inputColorSpace */,
	image::bitDepth /* inputDepth */,
	imbxUint32 inputHighBit,
	imbxInt32* pOutputBuffer,
	imbxUint32 buffersSize,
	image::bitDepth* /* pOutputDepth */,
	imbxUint32* pOutputHighBit
	)
{
	PUNTOEXE_FUNCTION_START(L"transformHighBit::doTransformBuffers");

	if(inputHighBit == *pOutputHighBit)
	{
		return;
	}

	if(inputHighBit < *pOutputHighBit)
	{
		imbxUint32 leftShift = *pOutputHighBit - inputHighBit;
		while(buffersSize--)
		{
			*(pOutputBuffer++) <<= leftShift;
		}
		return;
	}

	imbxUint32 rightShift = inputHighBit - *pOutputHighBit;
	while(buffersSize--)
	{
		*(pOutputBuffer++) >>= rightShift;
	}
	return;

	PUNTOEXE_FUNCTION_END();
}

} // namespace transforms

} // namespace imebra

} // namespace puntoexe
