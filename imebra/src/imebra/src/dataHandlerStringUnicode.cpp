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

/*! \file dataHandlerStringUnicode.cpp
    \brief Implementation of the base class used by the string handlers that need
	        to handle several charsets.

*/

#include "../../base/include/exception.h"
#include "../include/dataHandlerStringUnicode.h"


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
// dataHandlerStringUnicode
//
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

static dicomCharsetInformation m_dicomCharsets[]={

	dicomCharsetInformation(L"ISO 2022 IR 6",   "\x1b\x28\x42", "ISO-IR 6"),

	dicomCharsetInformation(L"ISO_IR 6", "", "ISO-IR 6"),
	dicomCharsetInformation(L"ISO_IR 100", "", "ISO-8859-1"),
	dicomCharsetInformation(L"ISO_IR 101", "", "ISO-8859-2"),
	dicomCharsetInformation(L"ISO_IR 109", "", "ISO-8859-3"),
	dicomCharsetInformation(L"ISO_IR 110", "", "ISO-8859-4"),
	dicomCharsetInformation(L"ISO_IR 144", "", "ISO-8859-5"),
	dicomCharsetInformation(L"ISO_IR 127", "", "ISO-8859-6"),
	dicomCharsetInformation(L"ISO_IR 126", "", "ISO-8859-7"),
	dicomCharsetInformation(L"ISO_IR 138", "", "ISO-8859-8"),
	dicomCharsetInformation(L"ISO_IR 148", "", "ISO-8859-9"),
	dicomCharsetInformation(L"ISO_IR 13",  "", "ISO-IR 13" ),
	dicomCharsetInformation(L"ISO_IR 166", "", "ISO-IR 166"),

	dicomCharsetInformation(L"", "\x1b\x28\x42", "ISO-IR 6"),
	dicomCharsetInformation(L"ISO 2022 IR 100", "\x1b\x2d\x41", "ISO-8859-1"),
	dicomCharsetInformation(L"ISO 2022 IR 101", "\x1b\x2d\x42", "ISO-8859-2"),
	dicomCharsetInformation(L"ISO 2022 IR 109", "\x1b\x2d\x43", "ISO-8859-3"),
	dicomCharsetInformation(L"ISO 2022 IR 110", "\x1b\x2d\x44", "ISO-8859-4"),
	dicomCharsetInformation(L"ISO 2022 IR 144", "\x1b\x2d\x4c", "ISO-8859-5"),
	dicomCharsetInformation(L"ISO 2022 IR 127", "\x1b\x2d\x47", "ISO-8859-6"),
	dicomCharsetInformation(L"ISO 2022 IR 126", "\x1b\x2d\x46", "ISO-8859-7"),
	dicomCharsetInformation(L"ISO 2022 IR 138", "\x1b\x2d\x48", "ISO-8859-8"),
	dicomCharsetInformation(L"ISO 2022 IR 148", "\x1b\x2d\x4d", "ISO-8859-9"),
	dicomCharsetInformation(L"ISO 2022 IR 13",  "\x1b\x29\x49", "ISO-IR 13"),
	dicomCharsetInformation(L"ISO 2022 IR 166", "\x1b\x2d\x54", "ISO-IR 166"),
	dicomCharsetInformation(L"ISO 2022 IR 87",  "\x1b\x24\x42", "ISO-IR 87"),
	dicomCharsetInformation(L"ISO 2022 IR 159", "\x1b\x24\x28\x44", "ISO-IR 159"),
	dicomCharsetInformation(L"ISO 2022 IR 149", "\x1b\x24\x29\x43", "ISO-IR 149"),

	dicomCharsetInformation(L"ISO_IR 192", "", "ISO-IR 192"),
	dicomCharsetInformation(L"GB18030",    "",    "GB18030"),

	dicomCharsetInformation(0,"","")
};


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Convert a string stored in a dicom tag to an unicode
//  string
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::wstring dataHandlerStringUnicode::convertToUnicode(std::string value)
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerStringUnicode::convertToUnicode");

	// Should we take care of the escape sequences...?
	///////////////////////////////////////////////////////////
	if(m_charsetsList.size() < 2)
	{
		// No, there isn't any escape sequence
		///////////////////////////////////////////////////////////
		return m_charsetConversion.toUnicode(value);
	}

	// We have to take care of the escape sequences.
	// Here we save the status of the conversion state.
	///////////////////////////////////////////////////////////
	saveCharsetConversionState saveConversionState(&m_charsetConversion);

	// Here we store the value to be returned
	///////////////////////////////////////////////////////////
	std::wstring returnString;

	// Scan all the string and look for valid escape sequences.
	// The partial strings are converted using the dicom
	//  charset specified by the escape sequences.
	///////////////////////////////////////////////////////////
	for(size_t scanString = 0; scanString < value.length(); /* empty */)
	{
		// Find the position of the next escape sequence
		///////////////////////////////////////////////////////////
		size_t escapePosition = value.length();
		std::string escapeString;
		std::string isoTable;
		for(int scanCharsets = 0; m_dicomCharsets[scanCharsets].m_dicomName != 0; ++scanCharsets)
		{
			std::string findEscapeString(m_dicomCharsets[scanCharsets].m_escapeSequence);
			if(findEscapeString == "")
			{
				continue;
			}
			size_t findEscape = value.find(findEscapeString, scanString);
			if(findEscape < escapePosition)
			{
				escapePosition = findEscape;
				escapeString = findEscapeString;
				isoTable = m_dicomCharsets[scanCharsets].m_isoRegistration;
			}
		}

		// The escape sequence can wait, now we are still in the
		//  already activated charset
		///////////////////////////////////////////////////////////
		if(escapePosition > scanString)
		{
			returnString += m_charsetConversion.toUnicode(value.substr(scanString, escapePosition - scanString));
		}

		// Move the char pointer to the next char that has to be
		//  analyzed
		///////////////////////////////////////////////////////////
		scanString = escapePosition + escapeString.length();

		// An iso table is coupled to the found escape sequence.
		///////////////////////////////////////////////////////////
		if(!isoTable.empty())
		{
			m_charsetConversion.initialize(isoTable);
		}
	}

	return returnString;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Convert an unicode string to a string ready to be
//  stored in a dicom tag
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::string dataHandlerStringUnicode::convertFromUnicode(std::wstring value)
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerStringUnicode::convertFromUnicode");

	// We don't have to deal with multiple charsets here
	///////////////////////////////////////////////////////////
	if(m_charsetsList.size() == 1)
	{
		dicomCharsetInformation* pCharset = getCharsetInfo(m_charsetsList.front());
		if(pCharset != 0 && pCharset->m_escapeSequence.empty())
		{
			return m_charsetConversion.fromUnicode(value);
		}
	}

	// Save the conversion state. Ready to deal with multiple
	//  charsets.
	///////////////////////////////////////////////////////////
	saveCharsetConversionState saveConversionState(&m_charsetConversion);

	// Returned string
	///////////////////////////////////////////////////////////
	std::string asciiString;
	
	// Convert all the chars. Each char is tested with the
	//  active charset first, then with other charsets if
	//  the active one doesn't work
	///////////////////////////////////////////////////////////
	size_t valueSize = value.size();
	for(size_t scanString = 0; scanString < valueSize; /* empty */)
	{
		// Find the last char that can be converted with the active
		//  charset
		///////////////////////////////////////////////////////////
		std::wstring code;
		size_t until;
		for(until = scanString; until < valueSize; /* empty */)
		{
			size_t increaseUntil = 1;
			code = value[until];
			
			// Check UTF-16 extension
			///////////////////////////////////////////////////////////
			if(sizeof(wchar_t) == 2)
			{
				if(value[until] >= 0xd800 && value[until] <=0xdfff && until < (valueSize - 1))
				{
					code += value[until+increaseUntil];
					++increaseUntil;
				}

				// Check composed chars extension
				///////////////////////////////////////////////////////////
				if((until + increaseUntil) < (valueSize - 1) && value[until + increaseUntil] >= 0x0300 && value[until + increaseUntil] <= 0x036f)
				{
					code += value[until+increaseUntil];
					++increaseUntil;
				}
			}
			
			// If the conversion doesn't succeed, exit from the loop
			///////////////////////////////////////////////////////////
			if(m_charsetConversion.fromUnicode(code).empty())
			{
				break;
			}
			until += increaseUntil;
		}

		// Convert all the chars that can be converted using the
		//  active charset
		///////////////////////////////////////////////////////////
		if(until > scanString)
		{
			std::wstring partialString = value.substr(scanString, until - scanString);
			asciiString += m_charsetConversion.fromUnicode(partialString);
			scanString = until;
		}

		// Exit if the end of the source string has been reached
		///////////////////////////////////////////////////////////
		if(until >= valueSize)
		{
			break;
		}

		// Find the escape sequence
		///////////////////////////////////////////////////////////
		std::string activeIso = m_charsetConversion.getIsoCharset();
		bool bSequenceFound = false;
		for(int scanCharsets = 0; m_dicomCharsets[scanCharsets].m_dicomName != 0; ++scanCharsets)
		{
			if(m_dicomCharsets[scanCharsets].m_escapeSequence.empty())
			{
				continue;
			}

			try
			{
				m_charsetConversion.initialize(m_dicomCharsets[scanCharsets].m_isoRegistration);
				if(!m_charsetConversion.fromUnicode(code).empty())
				{
					asciiString += m_dicomCharsets[scanCharsets].m_escapeSequence;
					bSequenceFound = true;

					// Add the dicom charset to the charsets
					///////////////////////////////////////////////////////////
					std::wstring dicomCharset = m_dicomCharsets[scanCharsets].m_dicomName;
					bool bAlreadyUsed = false;
					for(charsetsList::tCharsetsList::iterator scanUsedCharsets = m_charsetsList.begin(); scanUsedCharsets != m_charsetsList.end(); ++scanUsedCharsets)
					{
						if(*scanUsedCharsets == dicomCharset)
						{
							bAlreadyUsed = true;
							break;
						}
					}
					if(!bAlreadyUsed)
					{
						m_charsetsList.push_back(dicomCharset);
					}
					break;
				}
			}
			catch(charsetConversionExceptionNoSupportedTable)
			{
				continue;
			}
		}
		if(!bSequenceFound)
		{
			m_charsetConversion.initialize(activeIso);
			++scanString;
		}

	}

	return asciiString;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Return the information related to the requested
//  dicom charset
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
dicomCharsetInformation* dataHandlerStringUnicode::getCharsetInfo(std::wstring dicomName)
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerStringUnicode::getCharsetInfo");

	for(int scanCharsets = 0; m_dicomCharsets[scanCharsets].m_dicomName != 0; ++scanCharsets)
	{
		if(m_dicomCharsets[scanCharsets].m_dicomName == dicomName)
		{
			return &(m_dicomCharsets[scanCharsets]);
		}
	}
	return 0;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set the charset used in the tag
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandlerStringUnicode::setCharsetsList(charsetsList::tCharsetsList* pCharsetsList)
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerStringUnicode::setCharsetInfo");

	// Copy the specified charsets into the tag
	///////////////////////////////////////////////////////////
	m_charsetsList.clear();
	charsetsList::updateCharsets(pCharsetsList, &m_charsetsList);

	// If no charset has been defined then we use the default 
	//  one
	///////////////////////////////////////////////////////////
	if(m_charsetsList.empty())
	{
		m_charsetsList.push_back(m_dicomCharsets[0].m_dicomName);
	}

	// Check for the dicom charset's name
	///////////////////////////////////////////////////////////
	dicomCharsetInformation* pCharset = getCharsetInfo(m_charsetsList.front());
	if(pCharset == 0 || pCharset->m_isoRegistration.empty())
	{
		PUNTOEXE_THROW(dataHandlerStringUnicodeExceptionUnknownCharset, "Unknown charset");
	}

	// Setup the conversion objects
	///////////////////////////////////////////////////////////
	m_charsetConversion.initialize(pCharset->m_isoRegistration);
	m_localeCharsetConversion.initialize("LOCALE");

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Retrieve the dicom charsets used in the string
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandlerStringUnicode::getCharsetsList(charsetsList::tCharsetsList* pCharsetsList)
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerStringUnicode::getCharsetList");

	charsetsList::copyCharsets(&m_charsetsList, pCharsetsList);

	PUNTOEXE_FUNCTION_END();
}

} // namespace handlers

} // namespace imebra

} // namespace puntoexe
