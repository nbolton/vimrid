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

/*! \file dicomCodec.h
    \brief Declaration of the class dicomCodec.

*/

#if !defined(imebraDicomCodec_CC44A2C5_2B8C_42c1_9704_3F9C582643B9__INCLUDED_)
#define imebraDicomCodec_CC44A2C5_2B8C_42c1_9704_3F9C582643B9__INCLUDED_

#include "codec.h"
#include "data.h"
#include "dataGroup.h"
#include "dataSet.h"


/// \def IMEBRA_DATASET_MAX_DEPTH
/// \brief Max number of datasets embedded into each 
///        others.
///
/// This value is used to prevent a stack when reading a
///  malformed dataset that has too many datasets embedded
///   into each others.
///
///////////////////////////////////////////////////////////
#define IMEBRA_DATASET_MAX_DEPTH 16 

///////////////////////////////////////////////////////////
//
// Everything is in the namespace puntoexe::imebra
//
///////////////////////////////////////////////////////////
namespace puntoexe
{

namespace imebra
{

namespace codecs
{

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief The Dicom codec.
///
/// This class is used to decode and encode a DICOM stream.
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class dicomCodec : public codec
{
public:
	// Get an image from a dicom structure
	///////////////////////////////////////////////////////////
	virtual ptr<image> getImage(ptr<dataSet> pData, ptr<streamReader> pSourceStream, std::string dataType);

	// Write an image into a dicom structure
	///////////////////////////////////////////////////////////
	virtual void setImage(
		ptr<streamWriter> pDestStream,
		ptr<image> pImage,
		std::wstring transferSyntax,
		quality imageQuality,
		std::string dataType,
		imbxUint8 allocatedBits,
		bool bSubSampledX,
		bool bSubSampledY,
		bool bInterleaved,
		bool b2Complement);

	// Create another dicom codec
	///////////////////////////////////////////////////////////
	virtual ptr<codec> createCodec();

	/// \brief Parse the dicom stream and fill the data set
	///        with the read tags.
	///
	/// This function doesn't parse the 128 bytes preamble and
	///  the DICM signature, which has to be parsed by the
	///  calling function.
	///
	/// @param pStream    The stream do decode
	/// @param pDataSet   A pointer to the data set to fill
	///                    with the decoded tags
	/// @param bExplicitDataType true if the stream is encoded
	///                    with explicit data type, false
	///                    otherwise.
	///                   Even when this parameter is set,
	///                    the function will automatically
	///                    switch this parameter if a mismatch
	///                    is detected during the decoding
	///                    procedure
	/// @param endianType The stream's endian type.
	///                   Even when this parameter is set,
	///                    the function will automatically
	///                    switch this parameter if a mismatch
	///                    is detected during the decoding
	///                    procedure
	/// @param subItemLength the number of bytes to parse.
	///                   If this parameter is set to
	///                    0xffffffff then the function will
	///                    stop parsing at the end of the
	///                    sequence or at the end of the file
	/// @param maxSizeBufferLoad if a loaded buffer is exceedes
	///                    the size in the parameter then it is
	///                    not loaded immediatly but it will be
	///                    loaded on demand. Some codecs may
	///                    ignore this parameter.
	///                   Set to -1 to load all the buffers
	///                    immediatly
	/// @param pReadSubItemLength a pointer to a imbxUint32
	///                    that the function will fill with
	///                    the number of bytes read
	/// @param depth      the current dataSet depth:
	///                    - 0 = root dataset
	///                    - >=1 = dataset embedded into 
	///                      another dataset. This value is
	///                      used to prevent a stack overflow
	///
	///////////////////////////////////////////////////////////
	void parseStream(
		ptr<streamReader> pStream,
		ptr<dataSet> pDataSet,
		bool bExplicitDataType,
		streamController::tByteOrdering endianType,
		imbxUint32 maxSizeBufferLoad = 0xffffffff,
		imbxUint32 subItemLength = 0xffffffff,
		imbxUint32* pReadSubItemLength = 0,
		imbxUint32 depth = 0);

	/// \brief Write the dataSet to the specified stream
	///         in Dicom format, without the file header and
	///         signature.
	///
	/// @param pStream   the destination stream
	/// @param pDataSet  the dataSet to be written
	/// @param bExplicitDataType true if the function must
	///                   write the data type, false if
	///                   the data type is implicit
	/// @param endianType the endian type to be generated
	///
	///////////////////////////////////////////////////////////
	void buildStream(ptr<streamWriter> pStream, ptr<dataSet> pDataSet, bool bExplicitDataType, streamController::tByteOrdering endianType);

	// Returns true if the codec can handle the transfer
	//  syntax
	///////////////////////////////////////////////////////////
	virtual bool canHandleTransferSyntax(std::wstring transferSyntax);

	// Returns true if the transfer syntax has to be
	//  encapsulated
	//
	///////////////////////////////////////////////////////////
	virtual bool encapsulated(std::wstring transferSyntax);

	// Returns the maximum supported high bit
	///////////////////////////////////////////////////////////
	virtual imbxUint32 getMaxHighBit(std::string transferSyntax);

	// Returns the suggested allocated bits
	///////////////////////////////////////////////////////////
	virtual imbxUint32 suggestAllocatedBits(std::wstring transferSyntax, imbxUint32 highBit);

protected:
	// Write a dicom stream
	///////////////////////////////////////////////////////////
	virtual void writeStream(ptr<streamWriter> pStream, ptr<dataSet> pDataSet);

	// Load a dicom stream
	///////////////////////////////////////////////////////////
	virtual void readStream(ptr<streamReader> pStream, ptr<dataSet> pDataSet, imbxUint32 maxSizeBufferLoad = 0xffffffff);

protected:
	// Read a single tag
	///////////////////////////////////////////////////////////
	imbxUint32 readTag(ptr<streamReader> pStream, ptr<dataSet> pDataSet, imbxUint32 tagLengthDWord, imbxUint16 tagId, imbxUint16 order, imbxUint16 tagSubId, std::string, streamController::tByteOrdering endianType, short wordSize, imbxUint32 bufferId, imbxUint32 maxSizeBufferLoad = 0xffffffff);

	// Calculate the tag's length
	///////////////////////////////////////////////////////////
	imbxUint32 getTagLength(ptr<data> pData, bool bExplicitDataType, imbxUint32* pHeaderLength, bool *pbSequence);

	// Calculate the group's length
	///////////////////////////////////////////////////////////
	imbxUint32 getGroupLength(ptr<dataGroup>, bool bExplicitDataType);

	// Calculate the dataset's length
	///////////////////////////////////////////////////////////
	imbxUint32 getDataSetLength(ptr<dataSet>, bool bExplicitDataType);

	// Write a single group
	///////////////////////////////////////////////////////////
	void writeGroup(ptr<streamWriter> pDestStream, ptr<dataGroup> pGroup, imbxUint16 groupId, bool bExplicitDataType, streamController::tByteOrdering endianType);

	// Write a single tag
	///////////////////////////////////////////////////////////
	void writeTag(ptr<streamWriter> pDestStream, ptr<data> pData, imbxUint16 tagId, bool bExplicitDataType, streamController::tByteOrdering endianType);

	// Read an uncompressed interleaved image
	///////////////////////////////////////////////////////////
	void readUncompressedInterleaved(
		imbxUint32 channelsNumber,
		bool bSubSampledX,
		bool bSubSampledY,
		streamReader* pSourceStream,
		imbxUint8 wordSizeBytes,
		imbxUint8 allocatedBits,
		imbxUint32 mask
		);

	// Write an uncompressed interleaved image
	///////////////////////////////////////////////////////////
	void writeUncompressedInterleaved(
		imbxUint32 channelsNumber,
		bool bSubSampledX,
		bool bSubSampledY,
		streamWriter* pDestStream,
		imbxUint8 wordSizeBytes,
		imbxUint8 allocatedBits,
		imbxUint32 mask
		);

	// Read an uncompressed not interleaved image
	///////////////////////////////////////////////////////////
	void readUncompressedNotInterleaved(
		imbxUint32 channelsNumber,
		streamReader* pSourceStream,
		imbxUint8 wordSizeBytes,
		imbxUint8 allocatedBits,
		imbxUint32 mask
		);

	// Write an uncompressed not interleaved image
	///////////////////////////////////////////////////////////
	void writeUncompressedNotInterleaved(
		imbxUint32 channelsNumber,
		streamWriter* pDestStream,
		imbxUint8 wordSizeBytes,
		imbxUint8 allocatedBits,
		imbxUint32 mask
		);

	// Write an RLE compressed image
	///////////////////////////////////////////////////////////
	void writeRLECompressed(
		imbxUint32 imageSizeX,
		imbxUint32 imageSizeY,
		imbxUint32 channelsNumber,
		streamWriter* pDestStream,
		imbxUint8 allocatedBits,
		imbxUint32 mask
		);

	// Read an RLE compressed image
	///////////////////////////////////////////////////////////
	void readRLECompressed(
		imbxUint32 imageSizeX,
		imbxUint32 imageSizeY,
		imbxUint32 channelsNumber,
		streamReader* pSourceStream,
		imbxUint8 allocatedBits,
		imbxUint32 mask,
		bool bInterleaved);


	// Read a single pixel of a RAW dicom image
	///////////////////////////////////////////////////////////
	void readPixel(
					streamReader* pSourceStream,
					imbxInt32* pDest,
					imbxUint32 numPixels,
					imbxUint8* bitPointer,
					const imbxUint8 wordSizeBytes,
					const imbxUint8 allocatedBits,
					const imbxUint32 mask);

	// Write a single pixel of a RAW dicom image
	///////////////////////////////////////////////////////////
	void writePixel(
					streamWriter* pDestStream,
					imbxInt32 pixelValue,
					imbxUint8*  pBitPointer,
					imbxUint8 wordSizeBytes,
					imbxUint8 allocatedBits,
					imbxUint32 mask);

	// Flush the unwritten bytes of an uncompressed image
	///////////////////////////////////////////////////////////
	void flushUnwrittenPixels(streamWriter* pDestStream, imbxUint8* pBitPointer, imbxUint8 wordSizeBytes);

	imbxUint16 m_ioWord;
	imbxUint8  m_ioByte;

	void allocChannels(imbxUint32 channelsNumber, imbxUint32 sizeX, imbxUint32 sizeY, bool bSubSampledX, bool bSubSampledY);

	typedef ptr<channel> ptrChannel;
	std::vector<ptrChannel> m_channels;
};


/// \brief This is the base class for the exceptions thrown
///         by the dicom codec (dicomCodec).
///
///////////////////////////////////////////////////////////
class dicomCodecException: public codecException
{
public:
	/// \brief Build a dicomCodecException exception
	///
	/// @param message the message to store into the exception
	///
	///////////////////////////////////////////////////////////
	dicomCodecException(const std::string& message): codecException(message){}
};

/// \brief This exception is thrown when 
///         dicomCodec::parseStream reaches the maximum 
///         depth for embedded datasets.
///
///////////////////////////////////////////////////////////
class dicomCodecExceptionDepthLimitReached: public dicomCodecException
{
public:
	/// \brief Build a dicomCodecExceptionDepthLimitReached
	///        exception
	///
	/// @param message the message to store into the exception
	///
	///////////////////////////////////////////////////////////
	dicomCodecExceptionDepthLimitReached(const std::string&message): dicomCodecException(message){}

};



} // namespace codecs

} // namespace imebra

} // namespace puntoexe

#endif // !defined(imebraDicomCodec_CC44A2C5_2B8C_42c1_9704_3F9C582643B9__INCLUDED_)
