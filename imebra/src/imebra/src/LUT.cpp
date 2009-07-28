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

/*! \file LUT.cpp
    \brief Implementation of the class lut.

*/

#include "../../base/include/exception.h"
#include "../include/LUT.h"
#include "../include/dataHandler.h"
#include <string.h>

namespace puntoexe
{

namespace imebra
{

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Destructor
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
lut::~lut()
{
	if(m_pMappedValues)
	{
		delete m_pMappedValues;
	}
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Create a LUT from a data handler
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void lut::setLut(ptr<handlers::dataHandler> pDescriptor, ptr<handlers::dataHandler> pData, std::wstring description)
{
	PUNTOEXE_FUNCTION_START(L"lut::setLut");

	if(pDescriptor->getSize() < 3)
	{
		PUNTOEXE_THROW(lutExceptionCorrupted, "The LUT is corrupted");
	}
	pDescriptor->setPointer(0);
	imbxInt32 lutSize=pDescriptor->getSignedLongIncPointer();
	if(lutSize == 0)
		lutSize=0x00010000;
	if(lutSize < 0)
		lutSize&=0x0000FFFF;

	imbxInt32 lutFirstMapped=pDescriptor->getSignedLongIncPointer();
	imbxUint32 lutBits=pDescriptor->getUnsignedLongIncPointer();

	if(pData == 0 || (imbxUint32)lutSize != pData->getSize())
	{
		PUNTOEXE_THROW(lutExceptionCorrupted, "The LUT is corrupted");
	}

	create(lutSize, lutFirstMapped, (imbxUint8)lutBits, description);
	pData->copyToInt32(m_pMappedValues, lutSize);

	PUNTOEXE_FUNCTION_END();
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Create a LUT
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void lut::create(imbxUint32 size, imbxInt32 firstMapped, imbxUint8 bits, std::wstring description)
{
	PUNTOEXE_FUNCTION_START(L"lut::create");

	// If some values were previously allocated, then remove
	//  them
	///////////////////////////////////////////////////////////
	if(m_pMappedValues)
	{
		delete m_pMappedValues;
		m_pMappedValues= 0;
	}
	m_mappedValuesRev.clear();

	m_size = 0;

	m_description = description;

	if(size)
	{
		m_size=size;
		m_firstMapped=firstMapped;
		m_bits=bits;
		m_pMappedValues=new imbxInt32[m_size];
	}

	PUNTOEXE_FUNCTION_END();
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Fill the data handlers in a dataset with the lut
//  information
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void lut::fillHandlers(ptr<handlers::dataHandler> pDescriptor, ptr<handlers::dataHandler> pData)
{
	PUNTOEXE_FUNCTION_START(L"lut::fillHandlers");

	pDescriptor->setSize(3);
	pDescriptor->setPointer(0);
	imbxUint32 lutSize = getSize();
	if(lutSize == 0x00010000)
	{
		pDescriptor->setSignedLongIncPointer(0);
	}
	else
	{
		pDescriptor->setUnsignedLongIncPointer(lutSize);
	}

	imbxInt32 lutFirstMapped = getFirstMapped();
	pDescriptor->setSignedLongIncPointer(lutFirstMapped);

	imbxUint8 bits = getBits();
	pDescriptor->setUnsignedLong(bits);

	pData->setSize(lutSize);
	pData->copyFromInt32(m_pMappedValues, lutSize);

	PUNTOEXE_FUNCTION_END();
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Retrieve the number of values stored into the LUT
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxUint32 lut::getSize()
{
	return m_size;
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Retrieve the first mapped index
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxInt32 lut::getFirstMapped()
{
	return m_firstMapped;
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Insert a new value into the LUT
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void lut::setLutValue(imbxInt32 startValue, imbxInt32 lutValue)
{
	PUNTOEXE_FUNCTION_START(L"lut::setLutValue");

	if(startValue<m_firstMapped)
	{
		PUNTOEXE_THROW(lutExceptionWrongIndex, "The start index is below the first mapped index");
	}
	startValue-=m_firstMapped;
	if(startValue<(imbxInt32)m_size)
	{
		m_pMappedValues[startValue]=lutValue;
		m_mappedValuesRev.clear();
	}

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Retrieve the lut's description.
// The returned value must be removed by the client
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::wstring lut::getDescription()
{
	return m_description;
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Return the lut's bits
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxUint8 lut::getBits()
{
	return m_bits;
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Lookup the requested value
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxInt32 lut::mappedValue(imbxInt32 id)
{
	// The LUT's size is zero, return
	///////////////////////////////////////////////////////////
	if(m_size == 0)
	{
		return 0;
	}

	// Subtract the first mapped value
	///////////////////////////////////////////////////////////
	id -= m_firstMapped;

	if(id <= 0)
	{
		return m_pMappedValues[0];
	}
	if(id < (imbxInt32)m_size)
	{
		return m_pMappedValues[id];
	}
	return m_pMappedValues[m_size-1];
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Copy the palette's data to an array of imbxInt32
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void lut::copyToInt32(imbxInt32* pDestination, imbxUint32 destSize, imbxInt32* pFirstMapped)
{
	if(destSize > m_size)
	{
		destSize = m_size;
	}
	::memcpy(pDestination, m_pMappedValues, destSize*sizeof(imbxInt32));
	*pFirstMapped = m_firstMapped;
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Inverse Lookup table
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxInt32 lut::mappedValueRev(imbxInt32 lutValue)
{
	if(m_size == 0)
	{
		return 0;
	}

	if(m_mappedValuesRev.size() == 0)
	{
		for(imbxUint32 reverseLUT = 0; reverseLUT < m_size; ++reverseLUT)
		{
			m_mappedValuesRev[m_pMappedValues[reverseLUT]]=reverseLUT;
		}
	}

	if(lutValue<=m_mappedValuesRev.begin()->first)
		return m_mappedValuesRev.begin()->second;

	std::map<imbxInt32, imbxInt32>::iterator lutIterator;
	lutIterator=m_mappedValuesRev.end();
	--lutIterator;
	if(lutValue>=lutIterator->first)
		return lutIterator->second;

	lutIterator=m_mappedValuesRev.lower_bound(lutValue);

	if(lutIterator!=m_mappedValuesRev.end())
		return lutIterator->second;

	return 0;
}

} // namespace imebra

} // namespace puntoexe
