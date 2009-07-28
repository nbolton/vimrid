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

/*! \file dataHandlerString.cpp
    \brief Implementation of the base class for the string handlers.

*/

#include <sstream>
#include <iomanip>

#include "../../base/include/exception.h"
#include "../include/dataHandlerString.h"


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
// dataHandlerString
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
// Parse the buffer
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandlerString::parseBuffer(ptr<memory> memoryBuffer)
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerString::parseBuffer");

	m_strings.clear();

	// Convert the string to unicode
	///////////////////////////////////////////////////////////
	std::wstring tempBufferUnicode;
	imbxUint32 tempBufferSize(memoryBuffer->size());
	if(tempBufferSize != 0)
	{
		tempBufferUnicode = convertToUnicode(std::string((char*)(memoryBuffer->data()), tempBufferSize));
	}

	// Remove the extra spaces and zero bytes
	///////////////////////////////////////////////////////////
	size_t removeTrail;
	for(removeTrail = tempBufferUnicode.size(); removeTrail != 0 && (tempBufferUnicode[removeTrail - 1] == 0x20 || tempBufferUnicode[removeTrail - 1] == 0x0); --removeTrail){};
	tempBufferUnicode.resize(removeTrail);

	if( getSeparator() == 0)
	{
		m_strings.push_back(tempBufferUnicode);
		return;
	}

	size_t nextPosition;
	imbxUint32 unitSize=getUnitSize();
	for(size_t firstPosition = 0; firstPosition<tempBufferUnicode.size(); firstPosition = nextPosition)
	{
		nextPosition = tempBufferUnicode.find(getSeparator(), firstPosition);
		bool bSepFound=(nextPosition != tempBufferUnicode.npos);
		if(!bSepFound)
			nextPosition = tempBufferUnicode.size();

		if(unitSize && (nextPosition-firstPosition) > (size_t)unitSize)
			nextPosition=firstPosition+unitSize;

		m_strings.push_back(tempBufferUnicode.substr(firstPosition, nextPosition-firstPosition));

		if(bSepFound)
		{
			++nextPosition;
		}
	}

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Rebuild the buffer
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandlerString::buildBuffer(ptr<memory> memoryBuffer)
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerString::buildBuffer");

	std::wstring completeString;
	for(size_t stringsIterator = 0; stringsIterator < m_strings.size(); ++stringsIterator)
	{
		if(stringsIterator)
		{
			completeString += getSeparator();
		}
		completeString += m_strings[stringsIterator];
	}

	std::string asciiString = convertFromUnicode(completeString);

	memoryBuffer->assign((imbxUint8*)asciiString.c_str(), (imbxUint32)asciiString.size());

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set the data element's pointer
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandlerString::setPointer(imbxUint32 elementNumber)
{
	m_elementNumber=elementNumber;
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Increase the data element's pointer
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandlerString::incPointer()
{
	++m_elementNumber;
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Returns TRUE if the pointer is valid
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
bool dataHandlerString::pointerIsValid()
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerString::pointerIsValid");

	return m_elementNumber<m_strings.size();

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get data element as a signed long
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxInt32 dataHandlerString::getSignedLong()
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerString::getSignedLong");

	std::wstring tempString = getUnicodeString();
	std::wistringstream convStream(tempString);
	imbxInt32 value;
	convStream >> value;
	return value;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get data element as an unsigned long
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxUint32 dataHandlerString::getUnsignedLong()
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerString::getUnsignedLong");

	std::wstring tempString = getUnicodeString();
	std::wistringstream convStream(tempString);
	imbxUint32 value;
	convStream >> value;
	return value;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get data element as a double
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
double dataHandlerString::getDouble()
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerString::getDouble");

	std::wstring tempString = getUnicodeString();
	std::wistringstream convStream(tempString);
	double value;
	convStream >> value;
	return value;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get data element as a string
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::string dataHandlerString::getString()
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerString::getString");

	return convertFromUnicode(getUnicodeString());

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get data element as an unicode string
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::wstring dataHandlerString::getUnicodeString()
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerString::getUnicodeString");

	if(m_elementNumber >= m_strings.size())
	{
		return L"";
	}
	return m_strings[m_elementNumber];

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set data element as a signed long
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandlerString::setSignedLong(imbxInt32 value)
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerString::setSignedLong");

	std::wostringstream convStream;
	convStream << value;
	setUnicodeString(convStream.str());

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set data element as an unsigned long
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandlerString::setUnsignedLong(imbxUint32 value)
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerString::setUnsignedLong");

	std::wostringstream convStream;
	convStream << value;
	setUnicodeString(convStream.str());

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set data element as a double
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandlerString::setDouble(double value)
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerString::setDouble");

	std::wostringstream convStream;
	convStream << std::fixed << value;
	setUnicodeString(convStream.str());

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set data element as a string
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandlerString::setString(std::string value)
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerString::setString");

	setUnicodeString(convertToUnicode(value));

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set data element as an string
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandlerString::setUnicodeString(std::wstring value)
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerString::setUnicodeString");

	if(m_elementNumber<m_strings.size())
	{
		imbxUint32 stringMaxSize=maxSize();

		if(stringMaxSize && value.size()>stringMaxSize)
			value.resize(stringMaxSize);

		m_strings[m_elementNumber]=value;
	}

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set the buffer's size (in data elements)
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandlerString::setSize(imbxUint32 elementsNumber)
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerString::setSize");

	if(getSeparator() == 0)
	{
		elementsNumber=1L;
	}

	m_strings.resize(elementsNumber, L"");

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get the size in strings
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxUint32 dataHandlerString::getSize()
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerString::getSize");

	return (imbxUint32)m_strings.size();

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get the max size of a string
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxUint32 dataHandlerString::maxSize()
{
	return 0;
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Get the separator
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
wchar_t dataHandlerString::getSeparator()
{
	return L'\\';
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Convert a string to unicode, without using the dicom
//  charsets
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::wstring dataHandlerString::convertToUnicode(std::string value)
{
	size_t stringLength = value.size();
	std::wstring returnString;
	returnString.resize(stringLength, 0);
	for(size_t copyString = 0; copyString < stringLength; ++copyString)
	{
		returnString[copyString]=(wchar_t)value[copyString];
	}
	return returnString;
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Convert a string from unicode, without using the dicom 
//  charsets
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::string dataHandlerString::convertFromUnicode(std::wstring value)
{
	size_t stringLength = value.size();
	std::string returnString;
	returnString.resize(stringLength);
	for(size_t copyString = 0; copyString < stringLength; ++copyString)
	{
		returnString[copyString]=(char)value[copyString];
	}
	return returnString;
}


} // namespace handlers

} // namespace imebra

} // namespace puntoexe
