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

/*! \file charsetsList.h
    \brief Declaration of the the base class for the classes that need to be
            aware of the Dicom charsets.

*/

#if !defined(imebraCharsetsList_DE3F98A9_664E_47c0_A29B_B681F9AEB118__INCLUDED_)
#define imebraCharsetsList_DE3F98A9_664E_47c0_A29B_B681F9AEB118__INCLUDED_

#include <stdexcept>
#include <string>
#include <list>

namespace puntoexe
{

namespace imebra
{

namespace charsetsList
{

/// \brief Defines a list of widechar strings.
///
/// It is used to set or retrieve a list of charsets
///
///////////////////////////////////////////////////////////
typedef std::list<std::wstring> tCharsetsList;

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief This is the base class for the classes that
///         need to deal with the Dicom charsets.
///
/// The class supplies a list of charsets and exposes
///  two functions used to set or retrieve the list of
///  charsets.
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

void updateCharsets(tCharsetsList* pCharsetsList, tCharsetsList* pDestinationCharsetsList);
void copyCharsets(tCharsetsList* pSourceCharsetsList, tCharsetsList* pDestinationCharsetsList);

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief This is the base class for the exceptions thrown
///         by the class charsetsList.
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class charsetsListException: public std::runtime_error
{
public:
	charsetsListException(std::string message): std::runtime_error(message){}
};


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief This exception is thrown when a conversion from
///         an unicode string causes the dicom dataSet
///         to change its default charset.
///
/// For instace, the default charset is ISO IR 6 but a
///  value written by the application in one tag causes
///  the default charset to switch to ISO 2022 IR 100.
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class charsetListExceptionDiffDefault: public charsetsListException
{
public:
	charsetListExceptionDiffDefault(std::string message): charsetsListException(message){}
};

} // namespace charsetsList

} // namespace imebra

} // namespace puntoexe

#endif // !defined(imebraCharsetsList_DE3F98A9_664E_47c0_A29B_B681F9AEB118__INCLUDED_)

