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

/*! \file streamController.cpp
    \brief Implementation of the streamController class.

*/

#include "../include/streamController.h"

namespace puntoexe
{

// Used for the endian check
///////////////////////////////////////////////////////////
static const imbxUint16 m_endianCheck(0x00ff);
static imbxUint8 const * const pBytePointer((imbxUint8*)&m_endianCheck);
static const streamController::tByteOrdering m_platformByteOrder((*pBytePointer)==0xff ? streamController::lowByteEndian : streamController::highByteEndian);

///////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////
streamController::streamController(ptr<baseStream> pControlledStream, imbxUint32 virtualStart /* =0 */, imbxUint32 virtualLength /* =0 */):
	m_pTagByte(0),
        m_pControlledStream(pControlledStream),
		m_dataBuffer(new imbxUint8[IMEBRA_STREAM_CONTROLLER_MEMORY_SIZE]),
		m_virtualStart(virtualStart),
		m_virtualLength(virtualLength),
		m_dataBufferStreamPosition(0)
{
	m_pDataBufferStart = m_pDataBufferEnd = m_pDataBufferCurrent = m_dataBuffer.get();
	m_pDataBufferMaxEnd = m_pDataBufferStart + IMEBRA_STREAM_CONTROLLER_MEMORY_SIZE;
}


///////////////////////////////////////////////////////////
//
// Retrieve the current position
//
///////////////////////////////////////////////////////////
imbxUint32 streamController::position()
{
	return m_dataBufferStreamPosition + (imbxUint32)(m_pDataBufferCurrent - m_pDataBufferStart);
}


///////////////////////////////////////////////////////////
//
// Retrieve a pointer to the controlled stream
//
///////////////////////////////////////////////////////////
ptr<baseStream> streamController::getControlledStream()
{
	return m_pControlledStream;
}


///////////////////////////////////////////////////////////
//
// Retrieve the position without considering the virtual
//  start's position
//
///////////////////////////////////////////////////////////
imbxUint32 streamController::getControlledStreamPosition()
{
	return m_dataBufferStreamPosition + (imbxUint32)(m_pDataBufferCurrent - m_pDataBufferStart) + m_virtualStart;
}


///////////////////////////////////////////////////////////
//
// Adjust the byte ordering of pBuffer
//
///////////////////////////////////////////////////////////
void streamController::adjustEndian(imbxUint8* pBuffer, const imbxUint32 wordLength, const tByteOrdering endianType, const imbxUint32 words /* =1 */)
{
	if(endianType == m_platformByteOrder || wordLength<2L)
	{
		return;
	}

	switch(wordLength)
	{
	case 2:
		{ // Block needed by evc4. Prevent error on multiple definitions of scanWords
			imbxUint8 tempByte;
			for(imbxUint32 scanWords = words; scanWords != 0; --scanWords)
			{
				tempByte=*pBuffer;
				*pBuffer=*(pBuffer+1);
				*(++pBuffer)=tempByte;
				++pBuffer;
			}
		}
		return;
	case 4:
		{ // Block needed by evc4. Prevent error on multiple definitions of scanWords
			imbxUint8 tempByte0;
			imbxUint8 tempByte1;
			for(imbxUint32 scanWords = words; scanWords != 0; --scanWords)
			{
				tempByte0 = *pBuffer;
				*pBuffer = *(pBuffer+3);
				tempByte1 = *(++pBuffer);
				*pBuffer = *(pBuffer + 1);
				*(++pBuffer) = tempByte1;
				*(++pBuffer) = tempByte0;
				++pBuffer;
			}
		}
		return;
	}
}





} // namespace puntoexe
