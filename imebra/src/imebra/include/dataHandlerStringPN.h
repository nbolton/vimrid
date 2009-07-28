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

/*! \file dataHandlerStringPN.h
    \brief Declaration of the class dataHandlerStringPN.

*/

#if !defined(imebraDataHandlerStringPN_367AAE47_6FD7_4107_AB5B_25A355C5CB6E__INCLUDED_)
#define imebraDataHandlerStringPN_367AAE47_6FD7_4107_AB5B_25A355C5CB6E__INCLUDED_

#include "dataHandlerStringUnicode.h"


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

/*!
\brief Handles the Dicom type "PN" (person name).

This class separates the component groups of the name.

The person name can be formed by three groups:
 one or more groups can be absent.
- the first components group contains a character
  representation of the person name
- the second components group contains an ideographic
  representation of the person name
- the third components group contains a phonetic
  representation of the patient name

Inside a components group, the name components
 (first name, middle name, surname, etc) must be
 separated by a ^.

This class doesn't insert or parse the ^ separator
 which must be inserted and handled by the calling
 application, but handles the = separator which
 separates the components groups.\n
This means that to read or set all the patient name
 you have to move the pointer to the internal element
 by using setPointer(), incPointer() or skip().

For instance, to set the name "Left^Arrow" both
 with a character and an ideographic representation you
 have to use the following code:

\code
myDataSet->getDataHandler(group, 0, tag, 0, true, "PN");
myDataSet->setSize(2);
myDataSet->setUnicodeString(L"Left^Arrow");
myDataSet->incPointer();
myDataSet->setUnicodeString(L"<-"); // :-)
\endcode

*/
class dataHandlerStringPN : public dataHandlerStringUnicode
{
public:
	virtual imbxUint8 getPaddingByte();

	virtual imbxUint32 getUnitSize();

protected:
	// Return the maximum string's length
	///////////////////////////////////////////////////////////
	virtual imbxUint32 maxSize();

	// Returns the separator =
	///////////////////////////////////////////////////////////
	virtual wchar_t getSeparator();
};

} // namespace handlers

} // namespace imebra

} // namespace puntoexe

#endif // !defined(imebraDataHandlerStringPN_367AAE47_6FD7_4107_AB5B_25A355C5CB6E__INCLUDED_)
