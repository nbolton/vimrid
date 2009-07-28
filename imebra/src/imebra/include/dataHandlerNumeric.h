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

/*! \file dataHandlerNumeric.h
    \brief Declaration of the handler for the numeric tags.

*/

#if !defined(imebraDataHandlerNumeric_BD270581_5746_48d1_816E_64B700955A12__INCLUDED_)
#define imebraDataHandlerNumeric_BD270581_5746_48d1_816E_64B700955A12__INCLUDED_

#include <sstream>
#include <iomanip>

#include "../../base/include/exception.h"
#include "dataHandler.h"



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


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief This data handler accesses to the numeric data
///         stored in a puntoexe::imebra::buffer class.
///
/// A special definition of this class
///  (puntoexe::imebra::handlers::imageHandler) is used
///  to access to the images' raw pixels.
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
template<class dataHandlerType>
class dataHandlerNumeric : public dataHandler
{
	friend class buffer;
public:
	/// \brief Provides a direct access to the data managed by
	///         the handler.
	///
	/// @param nSubscript  the index of the numeric value that
	///                     you want to access to
	/// @return a reference to the requested value
	///
	///////////////////////////////////////////////////////////
	dataHandlerType& operator[](int nSubscript)
	{
		return m_pMemoryString[nSubscript];
	}

	/// \brief Provides a direct access to the data managed by
	///         the handler.
	///
	/// @param nSubscript  the index of the numeric value that
	///                     you want to access to
	/// @return a reference to the requested value
	///
	///////////////////////////////////////////////////////////
	dataHandlerType& at(int nSubscript)
	{
		return m_pMemoryString[nSubscript];
	}

	// Returns the size of an element managed by the
	//  handler.
	///////////////////////////////////////////////////////////
	virtual imbxUint32 getUnitSize()
	{
		return sizeof(dataHandlerType);
	}

	// Set the data element's pointer
	///////////////////////////////////////////////////////////
	virtual void setPointer(imbxUint32 elementNumber)
	{
		if(m_pMemoryString == 0 || elementNumber >= getSize())
		{
			m_elementPointer = 0;
		}
		m_elementPointer = &(m_pMemoryString[elementNumber]);
	}

	// Increase the data element's pointer
	///////////////////////////////////////////////////////////
	virtual void incPointer()
	{
		++m_elementPointer;
	}

	// Returns true if the pointer is valid
	///////////////////////////////////////////////////////////
	virtual bool pointerIsValid()
	{
		return m_elementPointer != 0 && m_elementPointer < (dataHandlerType*)m_pMemoryStringEnd;
	}

	// Increase the data element's pointer by the specified
	//  amount of steps
	///////////////////////////////////////////////////////////
	virtual void skip(int skip)
	{
		m_elementPointer+=skip;
	}

	// Retrieve the data element as a signed long
	///////////////////////////////////////////////////////////
	virtual imbxInt32 getSignedLong()
	{
		return (imbxInt32) (*m_elementPointer);
	}

	// Retrieve the data element an unsigned long
	///////////////////////////////////////////////////////////
	virtual imbxUint32 getUnsignedLong()
	{
		return (imbxUint32) (*m_elementPointer);
	}

	// Retrieve the data element as a double
	///////////////////////////////////////////////////////////
	virtual double getDouble()
	{
		return (double) (*m_elementPointer);
	}

	// Retrieve the data element as a signed long and increase
	//  the data element's pointer
	///////////////////////////////////////////////////////////
	virtual imbxInt32 getSignedLongIncPointer()
	{
		return (imbxInt32) (*(m_elementPointer++));
	}

	// Retrieve the data element as an unsigned long and
	//  increase the data element's pointer
	///////////////////////////////////////////////////////////
	virtual imbxUint32 getUnsignedLongIncPointer()
	{
		return (imbxUint32) (*(m_elementPointer++));
	}

	// Retrieve the data element as a double and increase the
	//  data element's pointer
	///////////////////////////////////////////////////////////
	virtual double getDoubleIncPointer()
	{
		return (double) (*(m_elementPointer++));
	}

	// Retrieve the data element as a string
	///////////////////////////////////////////////////////////
	virtual std::string getString()
	{
		PUNTOEXE_FUNCTION_START(L"dataHandlerNumeric::getString");

		std::ostringstream convStream;
		convStream << std::fixed << getDouble();
		return convStream.str();

		PUNTOEXE_FUNCTION_END();
	}

	// Retrieve the data element as a unicode string
	///////////////////////////////////////////////////////////
	virtual std::wstring getUnicodeString()
	{
		PUNTOEXE_FUNCTION_START(L"dataHandlerNumeric::getUnicodeString");

		std::wostringstream convStream;
		convStream << std::fixed << getDouble();
		return convStream.str();

		PUNTOEXE_FUNCTION_END();
	}

	// Retrieve the buffer's size in elements
	///////////////////////////////////////////////////////////
	virtual imbxUint32 getSize()
	{
		PUNTOEXE_FUNCTION_START(L"dataHandlerNumeric::getSize");

		return m_memory->size()/getUnitSize();

		PUNTOEXE_FUNCTION_END();
	}

	// Set the data element as a signed long
	///////////////////////////////////////////////////////////
	virtual void setSignedLong(imbxInt32 value)
	{
		*m_elementPointer = (dataHandlerType)value;
	}

	// Set the data element as an unsigned long
	///////////////////////////////////////////////////////////
	virtual void setUnsignedLong(imbxUint32 value)
	{
		*m_elementPointer = (dataHandlerType)value;
	}

	// Set the data element as a double
	///////////////////////////////////////////////////////////
	virtual void setDouble(double value)
	{
		*m_elementPointer = (dataHandlerType)value;
	}

	// Set the data element as a signed long and increase the
	//  data element's pointer
	///////////////////////////////////////////////////////////
	virtual void setSignedLongIncPointer(imbxInt32 value)
	{
		*(m_elementPointer++) = (dataHandlerType)value;
	}

	// Set the data element as an unsigned long and increase
	//  the data element's pointer
	///////////////////////////////////////////////////////////
	virtual void setUnsignedLongIncPointer(imbxUint32 value)
	{
		*(m_elementPointer++) = (dataHandlerType)value;
	}

	// Set the data element as a double and increase the data
	//  element's pointer
	///////////////////////////////////////////////////////////
	virtual void setDoubleIncPointer(double value)
	{
		*(m_elementPointer++) = (dataHandlerType)value;
	}

	// Set the data element as a string
	///////////////////////////////////////////////////////////
	virtual void setString(std::string value)
	{
		PUNTOEXE_FUNCTION_START(L"dataHandlerNumeric::setString");

		std::istringstream convStream(value);
		double tempValue;
		convStream >> tempValue;
		setDouble(tempValue);

		PUNTOEXE_FUNCTION_END();
	}

	// Set the data element as an unicode string
	///////////////////////////////////////////////////////////
	virtual void setUnicodeString(std::wstring value)
	{
		PUNTOEXE_FUNCTION_START(L"dataHandlerNumeric::setUnicodeString");

		std::wistringstream convStream(value);
		double tempValue;
		convStream >> tempValue;
		setDouble(tempValue);

		PUNTOEXE_FUNCTION_END();
	}

	/// \brief Returns a pointer to the physical %memory that
	///         stores the data managed by the handler.
	///
	/// Warning: the %memory that stores the managed data may
	///          be reallocated by a call to setSize().
	///
	/// @return a pointer to the physical %memory that stores
	///          the data managed by the handler
	///
	///////////////////////////////////////////////////////////
	dataHandlerType* getMemoryBuffer()
	{
		return m_pMemoryString;
	}

	/// \brief Return the memory object that stores the data
	///         managed by the handler.
	///
	/// @return the memory object that stores the data managed
	///          by the handler
	///
	///////////////////////////////////////////////////////////
	ptr<memory> getMemory()
	{
		return m_memory;
	}

	// Copy the data from an array of imbxInt32 values
	//  into the memory managed by the handler.
	///////////////////////////////////////////////////////////
	virtual void copyFromInt32(imbxInt32* pSource, imbxUint32 length)
	{
		PUNTOEXE_FUNCTION_START(L"dataHandlerNumeric::copyFromInt32");

		imbxUint32 myLength = getSize();
		if(length < myLength)
		{
			myLength = length;
		}
		dataHandlerType* pDest = m_pMemoryString;
		while(myLength--)
		{
			*(pDest++) = (dataHandlerType)( *(pSource++) );
		}

		PUNTOEXE_FUNCTION_END();
	}

	// Copy the data from the handler into an array of
	//  imbxInt32 values
	///////////////////////////////////////////////////////////
	virtual void copyToInt32(imbxInt32* pDest, imbxUint32 length)
	{
		PUNTOEXE_FUNCTION_START(L"dataHandlerNumeric::copyToInt32");

		imbxUint32 myLength = getSize();
		if(length < myLength)
		{
			myLength = length;
		}
		dataHandlerType* pSource = m_pMemoryString;
		while(myLength--)
		{
			*(pDest++) = (imbxInt32)( *(pSource++) );
		}

		PUNTOEXE_FUNCTION_END();
	}


	/// \brief Copy the content of an array of imbxInt32 values
	///         into the buffer controlled by the handler,
	///         considering that the source buffer could
	///         have subsampled data.
	///
	/// The source buffer is supposed to have the information
	///  related to a single channel.
	/// @param pSource      a pointer to the source array of
	///                      imbxInt32 values
	/// @param sourceReplicateX the horizontal subsamplig
	///                      factor of the source buffer
	///                      (1=not subsampled, 2=subsampled)
	/// @param sourceReplicateY the vertical subsamplig
	///                      factor of the source buffer
	///                      (1=not subsampled, 2=subsampled)
	/// @param destStartCol the horizontal coordinate of the
	///                      top left corner of the destination
	///                      rectangle
	/// @param destStartRow the vertical coordinate of the
	///                      top left corner of the destination
	///                      rectangle
	/// @param destEndCol   the horizontal coordinate of the
	///                      bottom right corner of the
	///                      destination rectangle
	/// @param destEndRow   the vertical coordinate of the
	///                      bottom right corner of the
	///                      destination rectangle
	/// @param destStartChannel the destination channel
	/// @param destWidth    the destination buffer's width in
	///                      pixels
	/// @param destHeight   the destination buffer's height in
	///                      pixels
	/// @param destNumChannels the number of channels in the
	///                      destination buffer
	///
	///////////////////////////////////////////////////////////
	virtual void copyFromInt32Interleaved(imbxInt32* pSource,
		imbxUint32 sourceReplicateX,
		imbxUint32 sourceReplicateY,
		imbxUint32 destStartCol,
		imbxUint32 destStartRow,
		imbxUint32 destEndCol,
		imbxUint32 destEndRow,
		imbxUint32 destStartChannel,
		imbxUint32 destWidth,
		imbxUint32 destHeight,
		imbxUint32 destNumChannels)
	{
		if(destStartCol >= destWidth || destStartRow >= destHeight)
		{
			return;
		}
		dataHandlerType *pDestRowScan = &(m_pMemoryString[(destStartRow*destWidth+destStartCol)*destNumChannels+destStartChannel]);
		imbxInt32* pSourceRowScan = pSource;

		imbxUint32 replicateXCount;
		imbxUint32 replicateYCount = sourceReplicateY;
		imbxUint32 replicateYIncrease = (destEndCol - destStartCol) / sourceReplicateX;

		dataHandlerType *pDestColScan;
		imbxInt32* pSourceColScan;

        if(destHeight < destEndRow)
        {
            destEndRow = destHeight;
        }
        if(destWidth < destEndCol)
        {
            destEndCol = destWidth;
        }
        imbxUint32 numColumns(destEndCol - destStartCol);
        imbxUint32 scanCol(0); // Used in the loop to scan the columns

		for(imbxUint32 numYCopies(destEndRow - destStartRow); numYCopies != 0; --numYCopies)
		{
			pDestColScan = pDestRowScan;
			pSourceColScan = pSourceRowScan;

            if(sourceReplicateX == 1)
            {
                for(scanCol = numColumns; scanCol != 0; --scanCol)
                {
                    *pDestColScan = (dataHandlerType)(*(pSourceColScan++));
                    pDestColScan += destNumChannels;
                }
            }
            else
            {
                replicateXCount = sourceReplicateX;
                for(scanCol = numColumns; scanCol != 0; --scanCol)
                {
                    *pDestColScan = (dataHandlerType)(*pSourceColScan);
                    pDestColScan += destNumChannels;
                    if(--replicateXCount == 0)
                    {
                        replicateXCount = sourceReplicateX;
                        ++pSourceColScan;
                    }
                }
            }
			pDestRowScan += destWidth * destNumChannels;
			if(--replicateYCount == 0)
			{
				replicateYCount = sourceReplicateY;
				pSourceRowScan += replicateYIncrease;
			}
		}
	}


	/// \brief Copy the buffer controlled by the handler into
	///         an array of imbxInt32 values, considering that
	///         the destination buffer could be subsampled
	///
	/// The destination buffer is supposed to have the
	///  information related to a single channel.
	/// @param pDest        a pointer to the destination array
	///                      of imbxInt32 values
	/// @param destSubSampleX the horizontal subsamplig
	///                      factor of the destination buffer
	///                      (1=not subsampled, 2=subsampled)
	/// @param destSubSampleY the vertical subsamplig
	///                      factor of the destination buffer
	///                      (1=not subsampled, 2=subsampled)
	/// @param sourceStartCol the horizontal coordinate of the
	///                      top left corner of the source
	///                      rectangle
	/// @param sourceStartRow the vertical coordinate of the
	///                      top left corner of the source
	///                      rectangle
	/// @param sourceEndCol   the horizontal coordinate of the
	///                      bottom right corner of the
	///                      source rectangle
	/// @param sourceEndRow   the vertical coordinate of the
	///                      bottom right corner of the
	///                      source rectangle
	/// @param sourceStartChannel the source channel to be
	///                      copied
	/// @param sourceWidth  the source buffer's width in
	///                      pixels
	/// @param sourceHeight the source buffer's height in
	///                      pixels
	/// @param sourceNumChannels the number of channels in the
	///                      source buffer
	///
	///////////////////////////////////////////////////////////
	virtual void copyToInt32Interleaved(imbxInt32* pDest,
		imbxUint32 destSubSampleX,
		imbxUint32 destSubSampleY,
		imbxUint32 sourceStartCol,
		imbxUint32 sourceStartRow,
		imbxUint32 sourceEndCol,
		imbxUint32 sourceEndRow,
		imbxUint32 sourceStartChannel,
		imbxUint32 sourceWidth,
		imbxUint32 sourceHeight,
		imbxUint32 sourceNumChannels)
	{
		if(sourceStartCol >= sourceWidth || sourceStartRow >= sourceHeight)
		{
			return;
		}
		dataHandlerType *pSourceRowScan = &(m_pMemoryString[(sourceStartRow*sourceWidth+sourceStartCol)*sourceNumChannels+sourceStartChannel]);
		imbxInt32* pDestRowScan = pDest;

		imbxUint32 subSampleXCount;
		imbxUint32 subSampleYCount = destSubSampleY;
		imbxUint32 subSampleYIncrease = (sourceEndCol - sourceStartCol) / destSubSampleX;

		dataHandlerType *pSourceColScan;
		imbxInt32* pDestColScan;

		imbxInt32 lastValue = (imbxInt32)*pSourceRowScan;

		for(imbxUint32 scanRow = sourceStartRow; scanRow < sourceEndRow; ++scanRow)
		{
			pSourceColScan = pSourceRowScan;
			pDestColScan = pDestRowScan;
			subSampleXCount = destSubSampleX;

			for(imbxUint32 scanCol = sourceStartCol; scanCol < sourceEndCol; ++scanCol)
			{
				if(scanCol < sourceWidth)
				{
					lastValue = (imbxInt32)*pSourceColScan;
					pSourceColScan += sourceNumChannels;
				}
				*pDestColScan += (imbxInt32)lastValue;
				if(--subSampleXCount == 0)
				{
					subSampleXCount = destSubSampleX;
					++pDestColScan;
				}
			}
			if(scanRow < sourceHeight - 1)
			{
				pSourceRowScan += sourceWidth * sourceNumChannels;
			}
			if(--subSampleYCount == 0)
			{
				subSampleYCount = destSubSampleY;
				pDestRowScan += subSampleYIncrease;
			}
		}

		imbxInt32 rightShift = 0;
		if(destSubSampleX == 2)
		{
			++rightShift;
		}
		if(destSubSampleY == 2)
		{
			++rightShift;
		}
		if(rightShift == 0)
		{
			return;
		}
		for(imbxInt32* scanDivide = pDest; scanDivide < pDestRowScan; ++scanDivide)
		{
			*scanDivide >>= rightShift;
		}

	}

	// Set the buffer's size, in data elements
	///////////////////////////////////////////////////////////
	virtual void setSize(imbxUint32 elementsNumber)
	{
		PUNTOEXE_FUNCTION_START(L"dataHandlerNumeric::setSize");

		m_memory->resize(elementsNumber * getUnitSize());
		m_pMemoryString = (dataHandlerType*)m_memory->data();
		m_pMemoryStringEnd = (imbxUint8*)m_pMemoryString + m_memory->size();
		setPointer(0);

		PUNTOEXE_FUNCTION_END();
	}

	// Parse the tag's buffer and extract its content
	///////////////////////////////////////////////////////////
	virtual void parseBuffer(ptr<memory> memoryBuffer)
	{
		PUNTOEXE_FUNCTION_START(L"dataHandlerNumeric::parseBuffer");

		m_memory = memoryBuffer;
		m_pMemoryString = (dataHandlerType*)memoryBuffer->data();
		m_pMemoryStringEnd = (imbxUint8*)m_pMemoryString + memoryBuffer->size();

		PUNTOEXE_FUNCTION_END();
	}

	// Rebuild the tag's buffer
	///////////////////////////////////////////////////////////
	virtual void buildBuffer(ptr<memory> memoryBuffer)
	{
		PUNTOEXE_FUNCTION_START(L"dataHandlerNumeric::buildBuffer");

		memoryBuffer->transfer(m_memory);

		PUNTOEXE_FUNCTION_END();
	}

protected:
	// Pointer to the data element
	///////////////////////////////////////////////////////////
	dataHandlerType* m_elementPointer;

	// Memory buffer
	///////////////////////////////////////////////////////////
	dataHandlerType* m_pMemoryString;
	const imbxUint8* m_pMemoryStringEnd;

	ptr<memory> m_memory;
};


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief A dataHandlerRaw always "sees" the data as a
///         collection of bytes, no matter what the Dicom
///         data type is.
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class dataHandlerRaw: public dataHandlerNumeric<imbxUint8>
{
};

} // namespace handlers

} // namespace imebra

} // namespace puntoexe

#endif // !defined(imebraDataHandlerNumeric_BD270581_5746_48d1_816E_64B700955A12__INCLUDED_)
