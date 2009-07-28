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

/*! \file streamWriter.h
    \brief Declaration of the the class used to write the streams.

*/


#if !defined(imebraStreamWriter_2C008538_F046_401C_8C83_2F76E1077DB0__INCLUDED_)
#define imebraStreamWriter_2C008538_F046_401C_8C83_2F76E1077DB0__INCLUDED_

#include "streamController.h"

///////////////////////////////////////////////////////////
//
// Everything is in the namespace puntoexe
//
///////////////////////////////////////////////////////////
namespace puntoexe
{

/// \brief Use this class to write into a baseStream
///         derived class.
///
/// Like the streamReader, this class is not multithread
///  safe, but several streamWriter (in several threads) 
///  can be connected to a single stream.
///
/// A streamWriter can also be connected only to a part
///  of the original stream: when this feature is used then
///  the streamWriter will act as if only the visible bytes
///  exist.
///
///////////////////////////////////////////////////////////
class streamWriter: public streamController
{
public:

	/// \brief Creates the streamWriter and connects it to a 
	///         baseStream object.
	///
	/// @param pControlledStream   the stream used by the 
	///                             streamWriter to write
	/// @param virtualStart        the first stream's byte
	///                             visible to the streamWriter
	/// @param virtualLength       the number of stream's bytes
	///                             visible to the 
	///                             streamWriter. Set to 0 to
	///                             allow the streamWriter to
	///                             see all the bytes
	///
	///////////////////////////////////////////////////////////
	streamWriter(ptr<baseStream> pControlledStream, imbxUint32 virtualStart = 0, imbxUint32 virtualLength = 0);

	/// \brief Flushes the internal buffer, disconnects the 
	///         stream and destroys the streamWriter.
	///
	///////////////////////////////////////////////////////////
	~streamWriter();

	/// \brief Writes the internal buffer into the connected
	///         stream. This function is automatically called
	///         when needed, but your application can call it
	///         when  syncronization between the cached data
	///         and the stream is needed.
	///
	///////////////////////////////////////////////////////////
	void flushDataBuffer();

	/// \brief Write raw data into the stream.
	///
	/// The data stored in the pBuffer parameter will be
	///  written into the stream.
	/// 
	/// The function throws a streamExceptionWrite exception
	///  if an error occurs.
	///
	/// @param pBuffer   a pointer to the buffer which stores
	///                   the data that must be written into
	///                   the stream
	/// @param bufferLength the number of bytes that must be
	///                   written to the stream
	///
	///////////////////////////////////////////////////////////
	void write(imbxUint8* pBuffer, imbxUint32 bufferLength);

	/// \brief Write the specified amount of bits to the
	///         stream.
	///
	/// The functions uses a special bit pointer to keep track
	///  of the bytes that haven't been completly written.
	///
	/// The function throws a streamExceptionWrite exception
	///  if an error occurs.
	///
	/// @param pBuffer   a pointer to a imbxUint32 value that
	///                   will be stores the bits to be written
	///                  The bits must be right aligned.
	/// @param bitsNum   the number of bits to write.
	///                  The function can write 32 bits maximum
	///                   
	///////////////////////////////////////////////////////////
	inline void writeBits(imbxUint32* const pBuffer, int bitsNum)
	{
		PUNTOEXE_FUNCTION_START(L"streamWriter::writeBits");

		imbxUint32 tempBuffer=*pBuffer;

		while(bitsNum != 0)
		{
			tempBuffer &= (((imbxUint32)1) << bitsNum) - 1;
			
			if(bitsNum <= (8 - m_outBitsNum))
			{
				m_outBitsBuffer |= (imbxUint8)(tempBuffer << (8 - m_outBitsNum - bitsNum));
				m_outBitsNum += bitsNum;
				bitsNum = 0;
			}
			else
			{
				m_outBitsBuffer |= (imbxUint8)(tempBuffer >> (bitsNum + m_outBitsNum - 8));
				bitsNum -= (8-m_outBitsNum);
				m_outBitsNum = 8;
			}

			if(m_outBitsNum==8)
			{
				m_outBitsNum = 0;
				writeByte(&m_outBitsBuffer);
				m_outBitsBuffer = 0;
			}
		}

		PUNTOEXE_FUNCTION_END();
	}

	/// \brief Reset the bit pointer used by writeBits().
	///
	/// A subsequent call to writeBits() will write data to
	///  a byte-aligned boundary.
	///
	///////////////////////////////////////////////////////////
	inline void resetOutBitsBuffer()
	{
		PUNTOEXE_FUNCTION_START(L"streamWriter::resetOutBitsBuffer");

		if(m_outBitsNum == 0)
			return;

		writeByte(&m_outBitsBuffer);
		flushDataBuffer();
		m_outBitsBuffer = 0;
		m_outBitsNum = 0;

		PUNTOEXE_FUNCTION_END();
	}

	/// \brief Write a single byte to the stream, parsing it
	///         if m_pTagByte is not zero.
	///
	/// The byte to be written must be stored in the buffer 
	///  pointed by the parameter pBuffer.
	///
	/// If m_pTagByte is zero, then the function writes 
	///  the byte and returns.
	///
	/// If m_pTagByte is not zero, then the function adds a
	///  byte with value 0x0 after all the bytes with value
	///  0xFF.
	/// This mechanism is used to avoid the generation of
	///  the jpeg tags in a stream.
	///
	/// @param pBuffer   a pointer to the location where the
	///                   value of the byte to be written
	///                   is stored
	///
	///////////////////////////////////////////////////////////
	inline void writeByte(imbxUint8* pBuffer)
	{
		if(m_pDataBufferCurrent == m_pDataBufferMaxEnd)
		{
			flushDataBuffer();
		}
		*(m_pDataBufferCurrent++) = *pBuffer;
		if(m_pTagByte != 0 && *pBuffer==(imbxUint8)0xff)
		{
			if(m_pDataBufferCurrent == m_pDataBufferMaxEnd)
			{
				flushDataBuffer();
			}
			*(m_pDataBufferCurrent++) = 0;
		}
	}

private:
	imbxUint8 m_outBitsBuffer;
	int       m_outBitsNum;

};

} // namespace puntoexe

#endif // !defined(imebraStreamWriter_2C008538_F046_401C_8C83_2F76E1077DB0__INCLUDED_)
