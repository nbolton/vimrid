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

/*! \file data.h
    \brief Declaration of class used to store a single tag.

*/

#if !defined(imebraData_20A41D11_C650_410b_B3AA_CD5B3FF63222__INCLUDED_)
#define imebraData_20A41D11_C650_410b_B3AA_CD5B3FF63222__INCLUDED_

#include "../../base/include/baseObject.h"

#include "charsetsList.h"

#include <map>
#include <vector>


///////////////////////////////////////////////////////////
//
// Everything is in the namespace puntoexe::imebra
//
///////////////////////////////////////////////////////////
namespace puntoexe
{
	class streamReader;
	class streamWriter;

namespace imebra
{

class buffer;
class dataSet;

namespace handlers
{
	class dataHandler;
	class dataHandlerRaw;
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief Represents a single Dicom Tag.
///
/// Dicom tags are stored into a dataSet object.
/// Each tag can be referred by its group ID and tag ID.
///
/// The class data uses the buffer objects to store its
///  %data.
///
/// Each tag can have several buffers (tags defined as
///  sequence in the dicom stream).
///
/// Your application can access to the tag's buffers
///  using a data handler (call getDataHandler()).
/// 
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class data : public baseObject
{
public:
	data(ptr<baseObject> externalLock): baseObject(externalLock) {}

	virtual ~data();

	///////////////////////////////////////////////////////////
	/// \name Data handlers
	///
	/// Retrieve dataHandler, dataHandlerRaw or a baseStream
	///  that allow to read or modify the buffer's data
	///
	///////////////////////////////////////////////////////////
	//@{

	/// \brief Returns true if the specified buffer exists, 
	///         otherwise it returns false.
	///
	/// The buffer can be created by requesting a writing
	///  handler (see handlers::dataHandler).
	///
	/// @param bufferId the zero-based buffer's id the 
	///                 function has to check for
	/// @return true if the buffer exists, false otherwise
	///
	///////////////////////////////////////////////////////////
	bool bufferExists(imbxUint32 bufferId);

	/// \brief Returns the size of a buffer, in bytes.
	///
	/// If the buffer doesn't exist then returns 0.
	/// See buffer::getBufferSizeBytes() for more info.
	///
	/// @param bufferId the zero-based buffer's id the 
	///                 function has to check for
	/// @return the buffer's size in bytes, or 0 if the buffer
	///          doesn't exist.
	///
	///////////////////////////////////////////////////////////
	imbxUint32 getBufferSize(imbxUint32 bufferId);

	/// \brief Get a data handler for the specified buffer.
	///
	/// handlers::dataHandler can be used to read, write 
	///  and resize the buffer.
	///
	/// handlers::dataHandler manages data types depending 
	///  on the buffer's data type: if your application
	///  has to work directly on the buffer's bytes,
	///  then it should obtain a handlers::dataHandlerRaw by
	///  calling getDataHandlerRaw().
	///
	/// Usually a tag has only one buffer (bufferId = 0), 
	///  but when a tag is a sequence or has an undefined
	///  length, then it could store more than one buffer;
	/// in this case your application has to specify the
	///  id of the buffer for which the handlers::dataHandler
	///  is requested.
	///
	/// The application can also specify a data type to use
	///  when a buffer doesn't exist yet: in this case a buffer
	///  with the specified data type will be allocated.
	///
	/// The returned handlers::dataHandler works on a local 
	///  copy of the buffer's data, then it doesn't have to 
	///  worry about multithreading accesses to the buffer's
	///  data.
	///
	/// @param bufferId the zero-based buffer's id the data
	///                 handler must use.
	/// @param bWrite   true if the application wants to write
	///                 something into the buffer
	/// @param defaultType a string that specifies the data
	///                  type to use for the new buffers.
	/// @return a pointer to the data handler for the
	///         requested buffer.
	///
	///////////////////////////////////////////////////////////
	ptr<handlers::dataHandler> getDataHandler(imbxUint32 bufferId, bool bWrite, std::string defaultType);
	
	/// \brief Get a raw data handler 
	///         (handlers::dataHandlerRaw) for the specified 
	///         buffer.
	///
	/// The raw data handler can be used to read, write and
	///  resize the buffer.
	///
	/// A raw data handler (handlers::dataHandlerRaw) always 
	///  works on a collection of bytes, no matter what the 
	///  buffer's data type is, while the normal 
	///  handlers::dataHandler works on data types depending 
	///  on the buffer's data type.
	///  
	/// A handlers::dataHandler can be obtained by calling
	///  getDataHandler().
	///
	/// Usually a tag has only one buffer (bufferId = 0), 
	///  but when a tag is a sequence or has an undefined
	///  length, then it could store more than one buffer;
	/// in this case your application has to specify the
	///  id of the buffer for which the data handler is
	///  requested.
	///
	/// The application can also specify a data type to use
	///  when a buffer doesn't exist yet: in this case a buffer
	///  with the specified data type will be allocated.
	///
	/// The returned handlers::dataHandlerRaw works on a local 
	///  copy of the buffer's data, then it doesn't have to 
	///  worry about multithreading accesses to the buffer's
	///  data.
	///
	/// @param bufferId the zero-based buffer's id the data
	///                 handler must use.
	/// @param bWrite   true if the application wants to write
	///                 something into the buffer
	/// @param defaultType a string that specifies the data
	///                  type to use for the new buffers.
	/// @return a pointer to the data handler for the
	///         requested buffer.
	///
	///////////////////////////////////////////////////////////
	ptr<handlers::dataHandlerRaw> getDataHandlerRaw(imbxUint32 bufferId, bool bWrite, std::string defaultType);

	/// \brief Get a streamReader connected to a buffer's data.
	///
	/// @param bufferId   the id of the buffer for which the
	///                    streamReader is required. This 
	///                    parameter is usually 0
	/// @return           the streamReader connected to the 
	///                    buffer's data.
	///
	///////////////////////////////////////////////////////////
	ptr<streamReader> getStreamReader(imbxUint32 bufferId);

	/// \brief Get a streamWriter connected to a buffer's data.
	///
	/// @param bufferId   the id of the buffer for which the
	///                    streamWriter is required. This 
	///                    parameter is usually 0
	/// @param dataType   the datatype used to create the 
	///                    buffer if it doesn't exist already
	/// @return           the streamWriter connected to the 
	///                    emptied buffer's data.
	///
	///////////////////////////////////////////////////////////
	ptr<streamWriter> getStreamWriter(imbxUint32 bufferId, std::string dataType = "");

	//@}


	///////////////////////////////////////////////////////////
	/// \name Embedded data sets.
	///
	/// Get or set an embedded dataSet
	///
	///////////////////////////////////////////////////////////
	//@{

	/// \brief Retrieve an embedded data set.
	///
	/// Sequence tags (dicom type=SQ) store embedded dicom 
	///  structures inside.
	///
	/// Sequences allow to nest several dicom structures.
	/// Sequences are used by dicom directories and by some
	///  tags in the normal dataSet.
	///
	/// @param dataSetId  the ID of the dataSet to retrieve.
	///                   Several data sets can be embedded
	///                   in the tag.
	///                   The first dataSet's ID is zero.
	/// @return           a pointer to the retrieved dataSet
	///
	///////////////////////////////////////////////////////////
	ptr<dataSet> getDataSet(imbxUint32 dataSetId);

	/// \brief Set an embedded dataSet to the sequence.
	///
	/// Several data sets can be nested one inside each other.
	/// When a data set is embedded into a tag, then the
	///  tag will have a sequence type (dicom type=SQ).
	///
	/// Sequences are used to build dicom directories and
	///  other tags in normal data sets.
	///
	/// @param dataSetId  The Id of the dataSet inside the tag.
	///                   Tags can store several data sets.
	///                   The first dataSet has an ID of zero
	/// @param pDataSet   A pointer to the dataSet to store
	///                   into the tag
	///
	///////////////////////////////////////////////////////////
	void setDataSet(imbxUint32 dataSetId, ptr<dataSet> pDataSet);

	/// \brief Append an embedded dataSet to the sequence.
	///
	/// Several data sets can be nested one inside each other.
	/// When a data set is embedded into a tag, then the
	///  tag will have a sequence type (dicom type=SQ).
	///
	/// Sequences are used to build dicom directories and
	///  other tags in normal data sets.
	///
	/// @param pDataSet   A pointer to the dataSet to store
	///                   into the tag
	///
	///////////////////////////////////////////////////////////
	void appendDataSet(ptr<dataSet> pDataSet);
	
	//@}


	///////////////////////////////////////////////////////////
	/// \name Buffer deletion
	///
	///////////////////////////////////////////////////////////
	//@{

	/// \brief Remove a buffer from the tag.
	///
	/// @param bufferId  the id of the buffer to be removed
	///                   from the tag
	///
	///////////////////////////////////////////////////////////
	void deleteBuffer(imbxUint32 bufferId);

	//@}


	///////////////////////////////////////////////////////////
	/// \name Buffer's data type
	///
	///////////////////////////////////////////////////////////
	//@{

	/// \brief Get the tag's buffer type in Dicom format.
	///
	/// A buffer's data type is composed by two uppercase
	///  chars.
	///
	/// @return the buffer's data type in Dicom format
	///
	///////////////////////////////////////////////////////////
	std::string getDataType();

	//@}

	virtual void setCharsetsList(charsetsList::tCharsetsList* pCharsetsList);
	virtual void getCharsetsList(charsetsList::tCharsetsList* pCharsetsList);

	// Set a buffer
	///////////////////////////////////////////////////////////
	void setBuffer(imbxUint32 bufferId, ptr<buffer> newBuffer);

protected:
	// Pointers to the internal buffers
	///////////////////////////////////////////////////////////
	typedef ptr<buffer> ptrBuffer;
	typedef std::map<imbxUint32, ptrBuffer> tBuffersMap;
	tBuffersMap m_buffers;

	// Pointers to the embedded datasets
	///////////////////////////////////////////////////////////
	typedef ptr<dataSet> ptrDataSet;
	typedef std::vector<ptrDataSet> tEmbeddedDatasetsMap;
	tEmbeddedDatasetsMap m_embeddedDataSets;

	charsetsList::tCharsetsList m_charsetsList;
};

} // namespace imebra

} // namespace puntoexe

#endif // !defined(imebraData_20A41D11_C650_410b_B3AA_CD5B3FF63222__INCLUDED_)
