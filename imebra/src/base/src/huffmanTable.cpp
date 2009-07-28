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

/*! \file huffmanTable.cpp
    \brief Implementation of the huffman codec.

*/

#include "../include/exception.h"
#include "../include/huffmanTable.h"
#include "../include/streamReader.h"
#include "../include/streamWriter.h"

#include <list>
#include <string.h>

namespace puntoexe
{

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
//
// huffmanTable
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
// Constructor
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
huffmanTable::huffmanTable(imbxUint32 maxValueLength)
{
	m_numValues=(1L<<(maxValueLength))+1L;

	reset();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Reset the class' attributes
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void huffmanTable::reset()
{
	PUNTOEXE_FUNCTION_START(L"huffmanTable::reset");

	m_valuesFreq.clear();
	m_valuesFreq.resize(m_numValues);

	m_orderedValues.resize(m_numValues);
	::memset(&(m_orderedValues[0]), 0, m_numValues*sizeof(m_orderedValues[0]));

	m_valuesToHuffman.resize(m_numValues);
	::memset(&(m_valuesToHuffman[0]), 0, m_numValues*sizeof(m_valuesToHuffman[0]));

	m_valuesToHuffmanLength.resize(m_numValues);
	::memset(&(m_valuesToHuffmanLength[0]), 0, m_numValues*sizeof(m_valuesToHuffmanLength[0]));

	::memset(m_valuesPerLength, 0, sizeof(m_valuesPerLength));

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Increase the frequency of one value
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void huffmanTable::incValueFreq(const imbxUint32 value)
{
	++(m_valuesFreq[value].m_freq);
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Remove the last huffman code from the lengths table
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void huffmanTable::removeLastCode()
{
	// Find the number of codes
	///////////////////////////////////////////////////////////
	imbxUint32 codes = 0;
	imbxUint32 lastLength = 0;
	for(imbxUint32 scanLengths = 0; scanLengths < sizeof(m_valuesPerLength)/sizeof(m_valuesPerLength[0]); ++scanLengths)
	{
		if(m_valuesPerLength[scanLengths] == 0)
		{
			continue;
		}
		codes += m_valuesPerLength[scanLengths];
		lastLength = scanLengths;
	}
	if(lastLength == 0)
	{
		return;
	}
	m_valuesPerLength[lastLength]--;
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Calculate the codes length
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void huffmanTable::calcHuffmanCodesLength(const imbxUint32 maxCodeLength)
{
	PUNTOEXE_FUNCTION_START(L"huffmanTable::calcHuffmanCodesLength");

	// Order the values by their frequency
	typedef std::map<huffmanTable::freqValue, bool, huffmanTable::freqValueCompare> tFreqOrderedMap;
	tFreqOrderedMap freqOrderedValues;

	for(size_t scanValues = 0; scanValues < m_valuesFreq.size(); ++scanValues)
	{
		if(m_valuesFreq[scanValues].m_freq != 0)
		{
			huffmanTable::freqValue key(m_valuesFreq[scanValues].m_freq, (imbxUint32)scanValues);
			freqOrderedValues[key] = true;
		}
	}

	while(freqOrderedValues.size() > 1)
	{
		huffmanTable::freqValue key0(freqOrderedValues.begin()->first);
		freqOrderedValues.erase(freqOrderedValues.begin());
		huffmanTable::freqValue key1(freqOrderedValues.begin()->first);
		freqOrderedValues.erase(freqOrderedValues.begin());

		key0.m_freq += key1.m_freq;
		m_valuesFreq[key0.m_value].m_freq = key0.m_freq;
		m_valuesFreq[key1.m_value].m_freq = 0;
		m_valuesFreq[key0.m_value].m_codeLength++;

		freqOrderedValues[key0] = true;

		imbxUint32 chainedValue;
		for(chainedValue = key0.m_value; m_valuesFreq[chainedValue].m_nextCode != -1; /* empty */)
		{
			chainedValue = (imbxUint32)m_valuesFreq[chainedValue].m_nextCode;
			m_valuesFreq[chainedValue].m_codeLength++;
		}
		m_valuesFreq[chainedValue].m_nextCode = key1.m_value;
		while(m_valuesFreq[chainedValue].m_nextCode != -1)
		{
			chainedValue = (imbxUint32)m_valuesFreq[chainedValue].m_nextCode;
			m_valuesFreq[chainedValue].m_codeLength++;
		}
	}

	typedef std::map<huffmanTable::lengthValue, bool, huffmanTable::lengthValueCompare> tLengthOrderedMap;
	tLengthOrderedMap lengthOrderedValues ;
	for(size_t findValuesPerLength = 0; findValuesPerLength < m_valuesFreq.size(); ++findValuesPerLength)
	{
		if(m_valuesFreq[findValuesPerLength].m_codeLength != 0)
		{
			huffmanTable::lengthValue key(m_valuesFreq[findValuesPerLength].m_codeLength, (imbxUint32)findValuesPerLength);
			lengthOrderedValues[key] = true;
			m_valuesPerLength[m_valuesFreq[findValuesPerLength].m_codeLength]++;
		}
	}

	long insertPosition = 0;
	for(tLengthOrderedMap::iterator scanLengths = lengthOrderedValues.begin(); scanLengths != lengthOrderedValues.end(); ++scanLengths)
	{
		m_orderedValues[insertPosition++] = scanLengths->first.m_value;
	}

	// Reduce the size of the codes' lengths
	for(imbxUint32 reduceLengths=sizeof(m_valuesPerLength)/sizeof(m_valuesPerLength[0]) - 1; reduceLengths>maxCodeLength; --reduceLengths)
	{
		while(m_valuesPerLength[reduceLengths] != 0)
		{
			imbxInt32 reduceLengths1;
			for(reduceLengths1=reduceLengths-2; reduceLengths1 != -1 && m_valuesPerLength[reduceLengths1] == 0; --reduceLengths1){}

			if(reduceLengths1==-1)
			{
				break;
			}
			m_valuesPerLength[reduceLengths]-=2;
			m_valuesPerLength[reduceLengths-1]++;
			m_valuesPerLength[reduceLengths1+1]+=2;
			m_valuesPerLength[reduceLengths1]--;
		}
	}

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Calculate the huffman codes from the codes length
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void huffmanTable::calcHuffmanTables()
{
	PUNTOEXE_FUNCTION_START(L"huffmanTable::calcHuffmanTables");

	imbxUint32 huffmanCode = 0;

	imbxUint32 valueIndex = 0;

	::memset(m_minValuePerLength, 0xffffffff, sizeof(m_minValuePerLength));
	::memset(m_maxValuePerLength, 0xffffffff, sizeof(m_maxValuePerLength));

	for(imbxUint32 codeLength=1L; codeLength<sizeof(m_valuesPerLength)/sizeof(imbxUint32); ++codeLength)
	{
		for(imbxUint32 generateCodes = 0; generateCodes<m_valuesPerLength[codeLength]; ++generateCodes)
		{
			if(generateCodes == 0)
			{
				m_minValuePerLength[codeLength]=huffmanCode;
			}
			m_maxValuePerLength[codeLength]=huffmanCode;
			m_valuesToHuffman[m_orderedValues[valueIndex]]=huffmanCode;
			m_valuesToHuffmanLength[m_orderedValues[valueIndex]]=codeLength;
			++valueIndex;
			++huffmanCode;
		}

		huffmanCode<<=1;

	}

	PUNTOEXE_FUNCTION_END();
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Read an Huffman code
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
bool huffmanTable::readHuffmanCode(imbxUint32* pBuffer, streamReader* pStream)
{
	PUNTOEXE_FUNCTION_START(L"huffmanTable::readHuffmanCode");

	// Buffer with the original huffman code
	///////////////////////////////////////////////////////////
	imbxUint32 readBuffer(0);

	// Used to add multiple bits to the read buffer
	///////////////////////////////////////////////////////////
    imbxUint32 addBitsReadBuffer(0);

	// Active ordered value
	///////////////////////////////////////////////////////////
	imbxUint32 orderedValue(0);

	// Number of bits to read in one go
	///////////////////////////////////////////////////////////
	imbxUint8 missingBits(0);

	// Scan all the codes sizes
	///////////////////////////////////////////////////////////
	for(imbxUint8 scanSize(1); scanSize<sizeof(m_valuesPerLength)/sizeof(m_valuesPerLength[0]); ++scanSize)
	{
        ++missingBits;

		// If the active length is empty, then continue the loop
		///////////////////////////////////////////////////////////
		if(m_valuesPerLength[scanSize] == 0)
		{
			continue;
		}

		// Read the missing bits
		///////////////////////////////////////////////////////////
		if(missingBits == scanSize) // Nothing in the buffer yet, we don't need to shift
		{
		    if(!pStream->readBits(&readBuffer, scanSize))
		    {
		        return false;
		    }
		}
		else
		{
		    if(missingBits == 1)
		    {
                if(!pStream->addBit(&readBuffer))
                {
                    return false;
                }
		    }
		    else
		    {
                if(!pStream->readBits(&addBitsReadBuffer, missingBits))
                {
                    return false;
                }
		        readBuffer <<= missingBits;
		        readBuffer |= addBitsReadBuffer;
		    }
		}

		// Reset the number of bits to read in one go
		///////////////////////////////////////////////////////////
		missingBits = 0;

		// Validate the current Huffman code. If it's OK, then
		//  return the ordered value
		///////////////////////////////////////////////////////////
		if(readBuffer<=m_maxValuePerLength[scanSize])
		{
			*pBuffer = m_orderedValues[orderedValue + readBuffer - m_minValuePerLength[scanSize]];
			return true;
		}
		orderedValue += m_valuesPerLength[scanSize];
	}

	*pBuffer = 0;

	PUNTOEXE_THROW(huffmanExceptionRead, "Invalid huffman code found while reading from a stream");

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Write an Huffman code
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void huffmanTable::writeHuffmanCode(imbxUint32* pBuffer, streamWriter* pStream)
{
	PUNTOEXE_FUNCTION_START(L"huffmanTable::writeHuffmanCode");

	if(m_valuesToHuffmanLength[*pBuffer] == 0)
	{
		PUNTOEXE_THROW(huffmanExceptionWrite, "Trying to write an invalid huffman code");
	}
	pStream->writeBits(&(m_valuesToHuffman[*pBuffer]), m_valuesToHuffmanLength[*pBuffer]);

	PUNTOEXE_FUNCTION_END();
}

} // namespace puntoexe

