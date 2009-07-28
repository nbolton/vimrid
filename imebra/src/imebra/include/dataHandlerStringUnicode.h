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

/*! \file dataHandlerStringUnicode.h
    \brief Declaration of the base class used by the string handlers that need to work
	        with different charsets.

*/

#if !defined(imebraDataHandlerStringUnicode_367AAE47_6FD7_4107_AB5B_25A355C5CB6E__INCLUDED_)
#define imebraDataHandlerStringUnicode_367AAE47_6FD7_4107_AB5B_25A355C5CB6E__INCLUDED_

#include "../../base/include/charsetConversion.h"
#include "dataHandlerString.h"
#include "charsetsList.h"



///////////////////////////////////////////////////////////
//
// Everything is in the namespace puntoexe::imebra
//
///////////////////////////////////////////////////////////
namespace puntoexe
{

namespace imebra
{

namespace handlers
{

struct dicomCharsetInformation
{
	dicomCharsetInformation(const wchar_t* dicomName, const char* escapeSequence, const char* isoRegistration):
		m_dicomName(dicomName), m_escapeSequence(escapeSequence), m_isoRegistration(isoRegistration){}
	const wchar_t* m_dicomName;
	std::string m_escapeSequence;
	std::string m_isoRegistration;
};

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief This is the base class for all the data handlers
///         that manage strings that need to be converted
///         from different unicode charsets
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class dataHandlerStringUnicode : public dataHandlerString
{
public:
	/// \internal
	/// \brief Defines the charset used by the strings encoded
	///         in the tag.
	///
	/// @param pCharsetsList the list of dicom charsets used by
	///                      the string
	/// 
	///////////////////////////////////////////////////////////
	virtual void setCharsetsList(charsetsList::tCharsetsList* pCharsetsList);
	
	/// \internal
	/// \brief Retrieve the charset used in the encoded string
	///
	/// @param pCharsetsList a list that will be filled with the
	///                      dicom charsets used in the string
	///
	///////////////////////////////////////////////////////////
	virtual void getCharsetsList(charsetsList::tCharsetsList* pCharsetsList);

protected:
	// Convert a string to unicode, using the dicom charsets
	///////////////////////////////////////////////////////////
	virtual std::wstring convertToUnicode(std::string value);

	// Convert a string from unicode, using the dicom charsets
	///////////////////////////////////////////////////////////
	virtual std::string convertFromUnicode(std::wstring value);

	charsetConversion m_charsetConversion;
	charsetConversion m_localeCharsetConversion;

	dicomCharsetInformation* getCharsetInfo(std::wstring dicomName);
};

class dataHandlerStringUnicodeException: public std::runtime_error
{
public:
	dataHandlerStringUnicodeException(std::string message): std::runtime_error(message){}
};

class dataHandlerStringUnicodeExceptionUnknownCharset: public dataHandlerStringUnicodeException
{
public:
	dataHandlerStringUnicodeExceptionUnknownCharset(std::string message): dataHandlerStringUnicodeException(message){}
};

} // namespace handlers

} // namespace imebra

} // namespace puntoexe

#endif // !defined(imebraDataHandlerStringUnicode_367AAE47_6FD7_4107_AB5B_25A355C5CB6E__INCLUDED_)
