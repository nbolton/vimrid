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

/*! \file dataHandler.h
    \brief Declaration of the base class used by all the data handlers.

*/

#if !defined(imebraDataHandler_6F85D344_DEF8_468d_BF73_AC5BB17FD22A__INCLUDED_)
#define imebraDataHandler_6F85D344_DEF8_468d_BF73_AC5BB17FD22A__INCLUDED_

#include "../../base/include/baseObject.h"
#include "buffer.h"
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

	class transaction; // is a friend

/// \namespace puntoexe::imebra::handlers
/// \brief All the data handlers returned by the class
///         buffer are defined in this namespace
///
///////////////////////////////////////////////////////////
namespace handlers
{

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief This is the base class for all the imebra data
///         handlers.
///        A data handler allows to read/write the data
///         stored in a \ref puntoexe::imebra::buffer 
///         object without worrying about the %data format.
///
/// Data handlers work on a local copy of the buffer,
///  then they don't need to worry about multithreading
///  accesses.
///
/// Also, once a dataHandler has been obtained from
///  a \ref buffer, it cannot be shared between threads
///  and it doesn't provide any multithread-safe mechanism,
///  except for its destructor which copies the local
///  buffer back to the original one (only for the writable
///  handlers).
///
/// Data handlers are also used to access to the
///  decompressed image's pixels (see image and 
///  handlers::imageHandler).
///
/// To obtain a data handler your application has to
///  call buffer::getDataHandler() or 
///  image::getDataHandler().
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class dataHandler : public baseObject
{
	// buffer & transaction are friend of this class
	///////////////////////////////////////////////////////////
	friend class puntoexe::imebra::buffer;
	friend class puntoexe::imebra::transaction;

public:
	// Contructor
	///////////////////////////////////////////////////////////
	dataHandler(): m_bCommitted(false){}

	// The data handler is disconnected
	///////////////////////////////////////////////////////////
	virtual bool preDelete();

	/// \brief In a writing handler copies back the modified
	///         data to the buffer.
	///
	/// Is not necessary to call this function directly because
	///  it is called by the handler's destructor, which copy
	///  any modification back to the buffer and finalize it.
	///
	/// The operation must be finalized by a call to commit(),
	///  or will be finalized by the destructor unless a call
	///  to abort() happen.
	///
	///////////////////////////////////////////////////////////
	void copyBack();
	
	/// \brief Finalize the copy of the data from the handler
	///         to the buffer.
	///
	/// Is not necessary to call this function directly because
	///  it is called by the handler's destructor, which copy
	///  any modification back to the buffer and then finalize
	///  it.
	///
	///////////////////////////////////////////////////////////
	void commit();

	/// \brief Discard all the changes made on a writing
	///         handler.
	///
	/// The function also switches the write flag to false,
	///  so it also prevent further changes from being
	///  committed into the buffer.
	///
	///////////////////////////////////////////////////////////
	void abort();

public:
	///////////////////////////////////////////////////////////
	/// \name Data pointer
	///
	/// The following functions set the value of the internal
	///  pointer that references the %data element read/written
	///  by the reading/writing functions.
	///
	///////////////////////////////////////////////////////////
	//@{

	/// \brief Move the internal pointer to the required.
	///         data element.
	///
	/// Subsequent calls to the reading/writing functions will
	///  read/write the element specified by using this
	///  function.
	///
	/// Note that the pointer is in "data elements", not in
	///  bytes.
	/// See also incPointer() and skip().
	///
	/// @param elementNumber The element number to activate.
	///
	///////////////////////////////////////////////////////////
	virtual void setPointer(imbxUint32 elementNumber) =0;

	/// \brief Increase the internal pointer by one element.
	///
	/// The internal pointer references the element that will
	///  be read/written by the reading/writing functions.
	///
	/// See also setPointer() and skip().
	///////////////////////////////////////////////////////////
	virtual void incPointer() =0;

	/// \brief Increase the data handler's internal pointer by
	///        the specified number of elements.
	///
	/// The internal pointer references the element that will
	///  be read/written by the reading/writing functions.
	///
	/// See also setPointer() and incPointer().
	///
	/// @param skip the number of element the to add to the
	///             current value of the data handler's
	///             internal pointer
	///////////////////////////////////////////////////////////
	virtual void skip(int skip);

	/// \brief Returns true if the data handler's internal
	///        pointer is valid.
	///
	/// The internal pointer references the element that will
	///  be read/written by the reading/writing functions.
	///
	/// If the internal pointer is out of the valid range, then
	///  this method returns false.
	///
	/// See also setPointer(), incPointer and skip().
	///
	/// @return the internal pointer's status (true=valid,
	///         false=invalid).
	///
	///////////////////////////////////////////////////////////
	virtual bool pointerIsValid() =0;

	//@}


	///////////////////////////////////////////////////////////
	/// \name Buffer and elements size
	///
	///////////////////////////////////////////////////////////
	//@{

	/// \brief Set the buffer's size, in elements.
	///
	/// The function resize the data handler's local buffer
	///  to the requested number of elements.
	///
	/// @param elementsNumber the requested buffer's size,
	///                        in data elements
	///
	///////////////////////////////////////////////////////////
	virtual void setSize(imbxUint32 elementsNumber) =0;

	/// \brief Retrieve the data handler's local buffer buffer
	///         size (in elements).
	///
	/// @return the buffer's size in elements
	///
	///////////////////////////////////////////////////////////
	virtual imbxUint32 getSize() =0;

	/// \brief Returns a single element's size in bytes.
	///
	/// If the element doesn't have a fixed size, then
	///  this function return 0.
	///
	/// @return the element's size in bytes, or zero if the
	///         element doesn't have a fixed size
	///
	///////////////////////////////////////////////////////////
	virtual imbxUint32 getUnitSize() =0;

	//@}
	

	///////////////////////////////////////////////////////////
	/// \internal
	/// \name Copy the local buffer from/to the buffer
	///
	///////////////////////////////////////////////////////////
	
	/// \internal
	/// \brief This function copies the %data from the 
	///         \ref buffer into the local %buffer
	///
	/// @param memoryBuffer the \ref memory managed by the 
	///                      \ref buffer
	///
	///////////////////////////////////////////////////////////
	virtual void parseBuffer(ptr<memory> memoryBuffer)=0;
	
	/// \internal
	/// \brief This function copies the %data from the 
	///         \ref buffer into the local %buffer
	///
	/// @param pBuffer a pointer to the %memory that stores the
	///                 data to be copied
	/// @param bufferLength the number of bytes stored in
	///                 pBuffer
	///
	///////////////////////////////////////////////////////////
	void parseBuffer(imbxUint8* pBuffer, imbxUint32 bufferLength);

	/// \internal
	/// \brief Copies the local %buffer into the original
	///         \ref buffer object.
	///
	/// @param memoryBuffer the \ref memory managed by the 
	///                      \ref buffer
	///
	///////////////////////////////////////////////////////////
	virtual void buildBuffer(ptr<memory> memoryBuffer)=0;

	/// \internal
	/// \brief Defines the charsets used in the string
	///
	/// @param pCharsetsList a list of dicom charsets
	///
	///////////////////////////////////////////////////////////
	virtual void setCharsetsList(charsetsList::tCharsetsList* pCharsetsList);

	/// \internal
	/// \brief Retrieve the charsets used in the string.
	///
	/// @param pCharsetsList a list that will be filled with the
	///                      dicom charsets used in the string
	///
	///////////////////////////////////////////////////////////
	virtual void getCharsetsList(charsetsList::tCharsetsList* pCharsetsList);

	//@}


	///////////////////////////////////////////////////////////
	/// \name Attributes
	///
	///////////////////////////////////////////////////////////
	//@{

	/// \brief Get the dicom data type managed by this handler.
	///
	/// The dicom data type is formed by 2 uppercase chars,
	///  as described by the dicom standard.
	/// See \ref buffer for further information.
	///
	/// @return the data handler's dicom data type
	///
	///////////////////////////////////////////////////////////
	std::string getDataType();

	/// \brief Return the byte that this handler uses to fill
	///         its content to make its size even.
	///
	/// @return the byte used to make the content's size even
	///
	///////////////////////////////////////////////////////////
	virtual imbxUint8 getPaddingByte();

	//@}


	///////////////////////////////////////////////////////////
	/// \name Reading/writing functions
	///
	///////////////////////////////////////////////////////////
	//@{

	/// \brief Retrieve the active %data element as a signed 
	///         long.
	///
	/// The active %data element is referenced by the %data
	///  handler's internal pointer, set by the method
	///  setPointer(), incPointer() and skip().
	///
	/// @return the value of the active data element
	///          transformed into a signed long
	///
	///////////////////////////////////////////////////////////
	virtual imbxInt32 getSignedLong() =0;

	/// \brief Retrieve the active %data element as an unsigned
	///         long.
	///
	/// The active %data element is referenced by the %data
	///  handler's internal pointer, set by the method
	///  setPointer(), incPointer() and skip().
	///
	/// @return the value of the active data element
	///          transformed into an unsigned long
	///
	///////////////////////////////////////////////////////////
	virtual imbxUint32 getUnsignedLong() =0;

	/// \brief Retrieve the active %data element as a double.
	///
	/// The active %data element is referenced by the %data
	///  handler's internal pointer, set by the method
	///  setPointer(), incPointer() and skip().
	///
	/// @return the value of the active data element
	///          transformed into a double
	///
	///////////////////////////////////////////////////////////
	virtual double getDouble() =0;

	/// \brief Retrieve the active %data element as a signed
	///         long and increase the %data handler's internal
	///         pointer by 1.
	///
	/// The active %data element is referenced by the data
	///  handler's internal pointer, set by the method
	///  setPointer(), incPointer() and skip().
	///
	/// @return the value of the active data element
	///         transformed into a signed long
	///
	///////////////////////////////////////////////////////////
	virtual imbxInt32 getSignedLongIncPointer();

	/// \brief Retrieve the active %data element as an unsigned
	///         long and increase the %data handler's internal
	///         pointer by 1.
	///
	/// The active %data element is referenced by the data
	///  handler's internal pointer, set by the method
	///  setPointer(), incPointer() and skip().
	///
	/// @return the value of the active data element
	///         transformed into an unsigned long
	///
	///////////////////////////////////////////////////////////
	virtual imbxUint32 getUnsignedLongIncPointer();

	/// \brief Retrieve the active %data element as a double
	///         and increase the %data handler's internal
	///         pointer by 1.
	///
	/// The active %data element is referenced by the data
	///  handler's internal pointer, set by the method
	///  setPointer(), incPointer() and skip().
	///
	/// @return the value of the active data element
	///         transformed into a double
	///
	///////////////////////////////////////////////////////////
	virtual double getDoubleIncPointer();
	
	/// \brief Retrieve the active %data element as a string
	///
	/// The retrieved string is converted using  the active 
	///  locale settings.
	/// The usage of getUnicodeString() is preferred
	///  because the retrieved string can represent all
	///  the chars stored in the dicom tag.
	///
	/// The active data element is referenced by the data
	///  handler's internal pointer, set by the method
	///  setPointer(), incPointer() and skip().
	///
	/// @return the value of the active data element
	///         transformed into a string.
	///
	///////////////////////////////////////////////////////////
	virtual std::string getString() = 0;

	/// \brief Retrieve the active %data element as an
	///         unicode string.
	///
	/// The chars used in the dicom tag are converted to
	///  unicode using the charsets defined in the dicom
	///  dataSet.
	///
	/// The active data element is referenced by the data
	///  handler's internal pointer, set by the method
	///  setPointer(), incPointer() and skip().
	///
	/// @return the value of the active data element
	///         transformed into a string.
	///
	///////////////////////////////////////////////////////////
	virtual std::wstring getUnicodeString() = 0;

	/// \brief Retrieve the active %data element as a date.
	///
	/// The active %data element is referenced by the data
	///  handler's internal pointer, set by the method
	///  setPointer(), incPointer() and skip().
	///
	/// @param pYear   a pointer to a value that will be filled
	///                 with the UTC date's year
	/// @param pMonth  a pointer to a value that will be filled
	///                 with the UTC date's month
	/// @param pDay    a pointer to a value that will be filled
	///                 with the UTC date's day of the month
	/// @param pHour   a pointer to a value that will be filled
	///                 with the UTC hour
	/// @param pMinutes a pointer to a value that will be 
	///                 filled with the UTC minutes
	/// @param pSeconds a pointer to a value that will be 
	///                 filled with the UTC seconds
	/// @param pNanoseconds a pointer to a value that will be 
	///                 filled with the UTC nanosecods
	/// @param pOffsetHours a pointer to a value that will be 
	///                 filled with the difference between the
	///                 date time zone and the UTC time zone
	/// @param pOffsetMinutes a pointer to a value that will be 
	///                 filled with the difference between the
	///                 date time zone and the UTC time zone
	///
	///////////////////////////////////////////////////////////
	virtual void getDate(
		imbxInt32* pYear, 
		imbxInt32* pMonth, 
		imbxInt32* pDay, 
		imbxInt32* pHour, 
		imbxInt32* pMinutes,
		imbxInt32* pSeconds,
		imbxInt32* pNanoseconds,
		imbxInt32* pOffsetHours,
		imbxInt32* pOffsetMinutes);

	/// \brief Set the active %data element as a date.
	///
	/// The active %data element is referenced by the data
	///  handler's internal pointer, set by the method
	///  setPointer(), incPointer() and skip().
	///
	/// @param year   the UTC date's year
	/// @param month  the UTC date's month
	/// @param day    the UTC date's day of the month
	/// @param hour   the UTC hour
	/// @param minutes the UTC minutes
	/// @param seconds the UTC seconds
	/// @param nanoseconds the UTC nanosecods
	/// @param offsetHours the difference between the date time 
	///                zone and the UTC time zone
	/// @param offsetMinutes the difference between the date
	///                time zone and the UTC time zone
	///
	///////////////////////////////////////////////////////////
	virtual void setDate(
		imbxInt32 year, 
		imbxInt32 month, 
		imbxInt32 day, 
		imbxInt32 hour, 
		imbxInt32 minutes,
		imbxInt32 seconds,
		imbxInt32 nanoseconds,
		imbxInt32 offsetHours,
		imbxInt32 offsetMinutes);

	/// \brief Set the active %data element from a signed
	///        long.
	///
	/// The active %data element is referenced by the data
	///  handler's internal pointer, set by the method
	///  setPointer(), incPointer() and skip().
	///
	/// @param value the value to write into the active
	///				  %data element.
	///
	///////////////////////////////////////////////////////////
	virtual void setSignedLong(imbxInt32 value) =0;

	/// \brief Set the active %data element from an unsigned
	///        long.
	///
	/// The active %data element is referenced by the data
	///  handler's internal pointer, set by the method
	///  setPointer(), incPointer() and skip().
	///
	/// @param value the value to write into the active
	///				  %data element.
	///
	///////////////////////////////////////////////////////////
	virtual void setUnsignedLong(imbxUint32 value) =0;

	/// \brief Set the active %data element from a double
	///
	/// The active %data element is referenced by the data
	///  handler's internal pointer, set by the method
	///  setPointer(), incPointer() and skip().
	///
	/// @param value the value to write into the active
	///				  %data element.
	///
	///////////////////////////////////////////////////////////
	virtual void setDouble(double value) =0;

	/// \brief Set the active %data element from a signed
	///         long and increase the data handler's internal
	///         pointer by one element.
	///
	/// The active %data element is referenced by the data
	///  handler's internal pointer, set by the method
	///  setPointer(), incPointer() and skip().
	///
	/// @param value the value to write into the active
	///				  %data element.
	///
	///////////////////////////////////////////////////////////
	virtual void setSignedLongIncPointer(imbxInt32 value);

	/// \brief Set the active %data element from an unsigned
	///         long and increase the data handler's internal
	///         pointer by one element.
	///
	/// The active %data element is referenced by the data
	///  handler's internal pointer, set by the method
	///  setPointer(), incPointer() and skip().
	///
	/// @param value the value to write into the active
	///				  %data element.
	///
	///////////////////////////////////////////////////////////
	virtual void setUnsignedLongIncPointer(imbxUint32 value);

	/// \brief Set the active %data element from a double
	///         and increase the data handler's internal
	///         pointer by one element.
	///
	/// The active %data element is referenced by the data
	///  handler's internal pointer, set by the method
	///  setPointer(), incPointer() and skip().
	///
	/// @param value the value to write into the active
	///				  %data element.
	///
	///////////////////////////////////////////////////////////
	virtual void setDoubleIncPointer(double value);

	/// \brief Set the active %data element from a string.
	///        SetUnicodeString() is preferred over 
	///         setString().
	///         
	/// The active %data element is referenced by the %data
	///  handler's internal pointer, set by the method
	///  setPointer(), incPointer() and skip().
	///
	/// @param value the value to write into the active
	///				  %data element.
	///////////////////////////////////////////////////////////
	virtual void setString(std::string value) =0;

	/// \brief Set the active %data element from an unicode
	///          string.
	///
	/// The unicode string is converted to multibyte using the
	///  dicom charsets when copied back to the tag's buffer.
	///
	/// The active %data element is referenced by the %data
	///  handler's internal pointer, set by the method
	///  setPointer(), incPointer() and skip().
	///
	/// @param value the value to write into the active
	///				  %data element.
	///////////////////////////////////////////////////////////
	virtual void setUnicodeString(std::wstring value) =0;

	//@}


	///////////////////////////////////////////////////////////
	/// \name Copy from/to imbxUint32 arrays
	///
	///////////////////////////////////////////////////////////
	//@{

	/// \brief Copy the content of an array of imbxInt32 values
	///         into the %buffer controlled by the handler.
	///
	/// The function doesn't resize the handler's %memory
	///  before copying the values. If the size of the source
	///  buffer is too big, then a partial copy will be
	///  performed.
	///
	/// @param pSource      the pointer to the array of 
	///                      imbxInt32 values.
	/// @param length       the number of imbxInt32 values that
	///                      must be copied from pSource into
	///                      the buffer controlled by the 
	///                      handler.
	///
	///////////////////////////////////////////////////////////
	virtual void copyFromInt32(imbxInt32* pSource, imbxUint32 length);
	
	/// \brief Copy the content of the %buffer controlled by
	///         the handler into an array of imbxInt32 values.
	///
	/// @param pDest        the pointer to the destination
	///                      array of imbxInt32 values
	/// @param length       the number of imbxInt32 values to
	///                      copy into the destination array
	///
	///////////////////////////////////////////////////////////
	virtual void copyToInt32(imbxInt32* pDest, imbxUint32 length);
	
	//@}

protected:
	// true if the buffer has been committed
	///////////////////////////////////////////////////////////
	bool m_bCommitted;

	// Pointer to the connected buffer
	///////////////////////////////////////////////////////////
	ptr<buffer> m_buffer;

	std::string m_bufferType;

	charsetsList::tCharsetsList m_charsetsList;
};

} // namespace handlers

} // namespace imebra

} // namespace puntoexe

#endif // !defined(imebraDataHandler_6F85D344_DEF8_468d_BF73_AC5BB17FD22A__INCLUDED_)
