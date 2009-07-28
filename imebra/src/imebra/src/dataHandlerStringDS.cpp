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

/*! \file dataHandlerStringDS.cpp
    \brief Implementation of the class dataHandlerStringDS.

*/

#include <sstream>
#include <iomanip>

#include "../../base/include/exception.h"
#include "../include/dataHandlerStringDS.h"

namespace puntoexe
{

namespace imebra
{

namespace handlers
{

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
//
// dataHandlerStringDS
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
// Get the value as a signed long.
// Overwritten to use getDouble()
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxInt32 dataHandlerStringDS::getSignedLong()
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerStringDS::getSignedLong");

	return (imbxInt32)getDouble();

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get the value as an unsigned long.
// Overwritten to use getDouble()
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxUint32 dataHandlerStringDS::getUnsignedLong()
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerStringDS::getUnsignedLong");

	return (imbxInt32)getDouble();

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set the value as a signed long.
// Overwritten to use setDouble()
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandlerStringDS::setSignedLong(imbxInt32 value)
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerStringDS::setSignedLong");

	setDouble((double)value);

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set the value as an unsigned long.
// Overwritten to use setDouble()
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandlerStringDS::setUnsignedLong(imbxUint32 value)
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerStringDS::setUnsignedLong");

	setDouble((double)value);

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Overwrite setDouble so the value is written in the
//  exponential form if needed
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandlerStringDS::setDouble(double value)
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerStringDS::setDouble");

	std::wostringstream convStream;
	convStream << value;
	setUnicodeString(convStream.str());

	PUNTOEXE_FUNCTION_END();
}

imbxUint8 dataHandlerStringDS::getPaddingByte()
{
	return 0x20;
}

imbxUint32 dataHandlerStringDS::getUnitSize()
{
	return 0;
}

imbxUint32 dataHandlerStringDS::maxSize()
{
	return 16;
}

} // namespace handlers

} // namespace imebra

} // namespace puntoexe
