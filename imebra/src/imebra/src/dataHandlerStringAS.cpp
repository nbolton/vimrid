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

/*! \file dataHandlerStringAS.cpp
    \brief Implementation of the class dataHandlerStringAS.

*/

#include <sstream>
#include <iomanip>

#include "../../base/include/exception.h"
#include "../include/dataHandlerStringAS.h"

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
// dataHandlerStringAS
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
// Set the age
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandlerStringAS::setAge(imbxUint32 age, tAgeUnit unit)
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerStringAS::setAge");

	std::wostringstream ageStream;
	ageStream << std::setfill(L'0');
	ageStream << std::setw(3) << age;
	ageStream << std::setw(1) << (wchar_t)unit;

	setUnicodeString(ageStream.str());

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Retrieve the age
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxUint32 dataHandlerStringAS::getAge(tAgeUnit* pUnit)
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerStringAS::getAge");

	std::wstring ageString = getUnicodeString();
	if(ageString.size() < 3)
	{
		ageString.resize(3, L'0');
	}
	if(ageString.size() < 4)
	{
		ageString.resize(4, L'Y');
	}
	std::wistringstream ageStream(ageString);
	imbxUint32 age;
	ageStream >> age;
	*pUnit = (tAgeUnit)ageString[3];

	return age;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Retrieve the age in years as a signed long
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxInt32 dataHandlerStringAS::getSignedLong()
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerStringAS::getSignedLong");

	return (imbxInt32)getDouble();

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Retrieve the age in years as an unsigned long
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxUint32 dataHandlerStringAS::getUnsignedLong()
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerStringAS::getUnsignedLong");

	return (imbxInt32)getDouble();

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Retrieve the age in years as a double
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
double dataHandlerStringAS::getDouble()
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerStringAS::getDouble");

	tAgeUnit ageUnit;
	double age = (double)getAge(&ageUnit);

	if(ageUnit == days)
	{
		return age / (double)365;
	}
	if(ageUnit == weeks)
	{
		return age / 52.14;
	}
	if(ageUnit == months)
	{
		return age / (double)12;
	}
	return age;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set the age in years as a signed long
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandlerStringAS::setSignedLong(imbxInt32 value)
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerStringAS::setSignedLong");

	setDouble((double)value);

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set the age in years as an unsigned long
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandlerStringAS::setUnsignedLong(imbxUint32 value)
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerStringAS::setUnsignedLong");

	setDouble((double)value);

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set the age in years as a double
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandlerStringAS::setDouble(double value)
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerStringAS::setDouble");

	if(value < 0)
	{
		value = 0;
	}
	if(value < 0.08)
	{
		setAge((imbxUint32)(value * 365), days);
		return;
	}
	if(value < 0.5)
	{
		setAge((imbxUint32)(value * 52.14), weeks);
		return;
	}
	if(value < 2)
	{
		setAge((imbxUint32)(value * 12), months);
		return;
	}
	setAge((imbxUint32)value, years);

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get the padding byte
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxUint8 dataHandlerStringAS::getPaddingByte()
{
	return 0x20;
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get the element's size
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxUint32 dataHandlerStringAS::getUnitSize()
{
	return 4L;
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get the maximum size
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxUint32 dataHandlerStringAS::maxSize()
{
	return 4L;
}

} // namespace handlers

} // namespace imebra

} // namespace puntoexe
