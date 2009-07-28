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

/*! \file charsetsList.cpp
    \brief Implementation of the basic class for the classes that handles the charsets.

*/

#include "../../base/include/exception.h"
#include "../include/charsetsList.h"


namespace puntoexe
{

namespace imebra
{

namespace charsetsList
{

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
//
// charsetsList
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
// Copy the charsets in the list into the local list.
// An exception is thrown if a mismatch in the default
//  charset is detected
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void updateCharsets(tCharsetsList* pCharsetsList, tCharsetsList* pDestinationCharsetsList)
{
	PUNTOEXE_FUNCTION_START(L"charsetsList::updateCharsets");

	// Check the default charset
	///////////////////////////////////////////////////////////
	if(!pCharsetsList->empty() && !pDestinationCharsetsList->empty() && pCharsetsList->front() != pDestinationCharsetsList->front())
	{
		PUNTOEXE_THROW(charsetListExceptionDiffDefault, "Different default charsets");
	}

	// Copy the charsets in the local list (if they are not
	//  already there)
	///////////////////////////////////////////////////////////
	for(tCharsetsList::iterator scanCharsets = pCharsetsList->begin(); scanCharsets != pCharsetsList->end(); ++scanCharsets)
	{
		bool bExist = false;
		for(tCharsetsList::iterator scanExistingCharsets = pDestinationCharsetsList->begin(); scanExistingCharsets != pDestinationCharsetsList->end(); ++scanExistingCharsets)
		{
			if(*scanCharsets == *scanExistingCharsets)
			{
				bExist = true;
				break;
			}
		}
		if(!bExist)
		{
			pDestinationCharsetsList->push_back(*scanCharsets);
		}
	}

	PUNTOEXE_FUNCTION_END();
}



///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Copy the charsets from the local list into the
//  specified list
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void copyCharsets(tCharsetsList* pSourceCharsetsList, tCharsetsList* pDestinationCharsetsList)
{
	PUNTOEXE_FUNCTION_START(L"charsetsList::copyCharsets");

	for(tCharsetsList::iterator scanCharsets = pSourceCharsetsList->begin(); scanCharsets != pSourceCharsetsList->end(); ++scanCharsets)
	{
		pDestinationCharsetsList->push_back(*scanCharsets);
	}

	PUNTOEXE_FUNCTION_END();
}

} // namespace charsetsList

} // namespace imebra

} // namespace puntoexe
