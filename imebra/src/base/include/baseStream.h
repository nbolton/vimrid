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

/*! \file baseStream.h
    \brief Declaration of the the base class for the streams (memory, file, ...)
            used by the puntoexe library.

*/

#if !defined(imebraBaseStream_3146DA5A_5276_4804_B9AB_A3D54C6B123A__INCLUDED_)
#define imebraBaseStream_3146DA5A_5276_4804_B9AB_A3D54C6B123A__INCLUDED_

#include "baseObject.h"
#include "../include/exception.h"
#include <vector>
#include <map>
#include <stdexcept>

///////////////////////////////////////////////////////////
//
// Everything is in the namespace puntoexe
//
///////////////////////////////////////////////////////////
namespace puntoexe
{

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief This class represents a stream.
///
/// Specialized classes derived from this class can
///  read/write from/to files stored on the computer's
///  disks, on the network or in memory.
///
/// The application can read or write into the stream
///  by using the streamReader or the streamWriter.
///
/// While this class can be used across several threads,
///  the streamReader and the streamWriter can be used in
///  one thread only. This is not a big deal, since one
///  stream can be connected to several streamReaders and
///  streamWriters.
///
/// The library supplies two specialized streams derived
///  from this class:
/// - puntoexe::stream (used to read or write into physical
///    files)
/// - puntoexe::memoryStream (used to read or write into
///    puntoexe::memory objects)
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class baseStream : public baseObject
{

public:
	/// \brief Writes raw data into the stream.
	///
	/// The function is multithreading-safe and is called by
	///  the streamWriter class when its buffer has to be
	///  flushed.
	///
	/// @param startPosition  the position in the file where
	///                        the data has to be written
	/// @param pBuffer        pointer to the data that has to
	///                        be written
	/// @param bufferLength   number of bytes in the data 
	///                        buffer that has to be written
	///
	///////////////////////////////////////////////////////////
	virtual void write(imbxUint32 startPosition, imbxUint8* pBuffer, imbxUint32 bufferLength) = 0;
	
	/// \brief Read raw data from the stream.
	///
	/// The function is multithreading-safe and is called by
	///  the streamReader class when its buffer has to be
	///  refilled.
	///
	/// @param startPosition  the position in the file from
	///                        which the data has to be read
	/// @param pBuffer        a pointer to the memory where the
	///                        read data has to be placed
	/// @param bufferLength   the number of bytes to read from
	///                        the file
	/// @return the number of bytes read from the file. When
	///          it is 0 then the end of the file has been
	///          reached
	///
	///////////////////////////////////////////////////////////
	virtual imbxUint32 read(imbxUint32 startPosition, imbxUint8* pBuffer, imbxUint32 bufferLength) = 0;
};


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief The base exception for all the exceptions
///         thrown by the function in baseStream.
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class streamException: public std::runtime_error
{
public:
	streamException(std::string message): std::runtime_error(message){}
};


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief Exception thrown when the stream cannot be
///         open.
///
///////////////////////////////////////////////////////////
class streamExceptionOpen : public streamException
{
public:
	streamExceptionOpen(std::string message): streamException(message){}
};


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief Exception thrown when there is an error during
///         the read phase.
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class streamExceptionRead : public streamException
{
public:
	streamExceptionRead(std::string message): streamException(message){}
};


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief Exception thrown when there is an error during
///         the write phase.
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class streamExceptionWrite : public streamException
{
public:
	streamExceptionWrite(std::string message): streamException(message){}
};


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief Exception thrown when there are problems during
///         the closure of the stream.
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class streamExceptionClose : public streamException
{
public:
	streamExceptionClose(std::string message): streamException(message){}
};

} // namespace puntoexe


#endif // !defined(imebraBaseStream_3146DA5A_5276_4804_B9AB_A3D54C6B123A__INCLUDED_)
