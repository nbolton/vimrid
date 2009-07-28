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

/*! \file dataHandlerTime.cpp
    \brief Implementation of the class dataHandlerTime.

*/

#include <sstream>
#include <iomanip>
#include <stdlib.h>

#include "../../base/include/exception.h"
#include "../include/dataHandlerTime.h"

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
// dataHandlerTime
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
// Get the maximum size
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxUint32 dataHandlerTime::maxSize()
{
	return 16;
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get the fixed size
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxUint32 dataHandlerTime::getUnitSize()
{
	return 0;
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Parse the buffer and build the local buffer
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandlerTime::parseBuffer(ptr<memory> memoryBuffer)
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerTime::parseBuffer");

	// Parse the string
	///////////////////////////////////////////////////////////
	dataHandlerString::parseBuffer(memoryBuffer);

	// Adjust the parsed string so it is a legal time
	///////////////////////////////////////////////////////////
	setPointer(0);
	std::wstring unicodeString;
	if(pointerIsValid())
	{
		unicodeString = dataHandlerString::getUnicodeString();
	}

	// Remove trailing spaces an invalid chars
	///////////////////////////////////////////////////////////
	size_t removeTrail;
	for(removeTrail = unicodeString.size(); removeTrail != 0 && ( unicodeString[removeTrail - 1] == 0x20 || unicodeString[removeTrail - 1] == 0x0); --removeTrail){};
	unicodeString.resize(removeTrail);

	// Separate the components
	///////////////////////////////////////////////////////////
	std::vector<std::wstring> components;
	split(unicodeString, L":", &components);
	std::wstring normalizedTime;

	if(components.size() == 1)
	{
		normalizedTime = components.front();
	}
	else
	{
		if(components.size() > 0)
		{
			normalizedTime = padLeft(components[0], L"0", 2);
		}
		if(components.size() > 1)
		{
			normalizedTime += padLeft(components[1], L"0", 2);
		}
		if(components.size() > 2)
		{
			std::vector<std::wstring> secondsComponents;
			split(components[2], L".", &secondsComponents);
			if(secondsComponents.size() > 0)
			{
				normalizedTime += padLeft(secondsComponents[0], L"0", 2);
			}
			if(secondsComponents.size() > 1)
			{
				normalizedTime += L'.';
				normalizedTime += secondsComponents[1];
			}
		}
	}
	dataHandlerString::setUnicodeString(normalizedTime);

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set the date/time
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandlerTime::setDate(
		 imbxInt32 /* year */,
		 imbxInt32 /* month */,
		 imbxInt32 /* day */,
		 imbxInt32 hour,
		 imbxInt32 minutes,
		 imbxInt32 seconds,
		 imbxInt32 nanoseconds,
		 imbxInt32 offsetHours,
		 imbxInt32 offsetMinutes)
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerTime::setDate");

	std::wstring timeString = buildTime(hour, minutes, seconds, nanoseconds, offsetHours, offsetMinutes);
	timeString.resize(13);
	dataHandlerDateTimeBase::setUnicodeString(timeString);

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get the date/time
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandlerTime::getDate(
		 imbxInt32* pYear,
		 imbxInt32* pMonth,
		 imbxInt32* pDay,
		 imbxInt32* pHour,
		 imbxInt32* pMinutes,
		 imbxInt32* pSeconds,
		 imbxInt32* pNanoseconds,
		 imbxInt32* pOffsetHours,
		 imbxInt32* pOffsetMinutes)
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerTime::getDate");

	*pYear = 0;
	*pMonth = 0;
	*pDay = 0;
	*pHour = 0;
	*pMinutes = 0;
	*pSeconds = 0;
	*pNanoseconds = 0;
	*pOffsetHours = 0;
	*pOffsetMinutes = 0;

	std::wstring timeString=dataHandlerDateTimeBase::getUnicodeString();
	parseTime(timeString, pHour, pMinutes, pSeconds, pNanoseconds, pOffsetHours, pOffsetMinutes);

	PUNTOEXE_FUNCTION_END();

}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get a string representation of the time
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::wstring dataHandlerTime::getUnicodeString()
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerTime::getUnicodeString");

	imbxInt32 year, month, day, hour, minutes, seconds, nanoseconds, offsetHours, offsetMinutes;
	getDate(&year, &month, &day, &hour, &minutes, &seconds, &nanoseconds, &offsetHours, &offsetMinutes);

	std::wostringstream convStream;
	convStream << std::setfill(L'0');
	convStream << std::setw(2) << hour;
	convStream << std::setw(1) << L":";
	convStream << std::setw(2) << minutes;
	convStream << std::setw(1) << L":";
	convStream << std::setw(2) << seconds;
	convStream << std::setw(1) << L".";
	convStream << std::setw(6) << nanoseconds;
	if(offsetHours != 0 && offsetMinutes != 0)
	{
		convStream << std::setw(1) << (offsetHours < 0 ? L"-" : L"+");
		convStream << std::setw(2) << labs(offsetHours);
		convStream << std::setw(1) << L":";
		convStream << std::setw(2) << labs(offsetMinutes);
	}

	return convStream.str();

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set a string representation of the time
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandlerTime::setUnicodeString(std::wstring value)
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerTime::setUnicodeString");

	std::vector<std::wstring> components;
	split(value, L"-/.: +-", &components);

	imbxInt32 year, month, day, hour, minutes, seconds, nanoseconds, offsetHours, offsetMinutes;
	year = 0;
	month = 1;
	day = 1;
	hour = 0;
	minutes = 0;
	seconds = 0;
	nanoseconds = 0;
	offsetHours = 0;
	offsetMinutes = 0;

	if(components.size() > 0)
	{
		std::wistringstream convStream(components[0]);
		convStream >> hour;
	}

	if(components.size() > 1)
	{
		std::wistringstream convStream(components[1]);
		convStream >> minutes;
	}

	if(components.size() > 2)
	{
		std::wistringstream convStream(components[2]);
		convStream >> seconds;
	}

	if(components.size() > 3)
	{
		std::wistringstream convStream(components[3]);
		convStream >> nanoseconds;
	}

	if(components.size() > 4)
	{
		std::wistringstream convStream(components[4]);
		convStream >> offsetHours;
	}

	if(components.size() > 5)
	{
		std::wistringstream convStream(components[5]);
		convStream >> offsetMinutes;
	}

	if(value.find(L'+') == value.npos)
	{
		offsetHours = -offsetHours;
		offsetMinutes = -offsetMinutes;
	}


	setDate(year, month, day, hour, minutes, seconds, nanoseconds, offsetHours, offsetMinutes);

	PUNTOEXE_FUNCTION_END();
}


} // namespace handlers

} // namespace imebra

} // namespace puntoexe
