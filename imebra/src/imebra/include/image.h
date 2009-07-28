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

/*! \file image.h
    \brief Declaration of the class image.

*/

#if !defined(imebraImage_A807A3CA_FA04_44f4_85D2_C7AA2FE103C4__INCLUDED_)
#define imebraImage_A807A3CA_FA04_44f4_85D2_C7AA2FE103C4__INCLUDED_

#include "../../base/include/baseObject.h"
#include "dataHandlerNumeric.h"


///////////////////////////////////////////////////////////
//
// Everything is in the namespace puntoexe::imebra
//
///////////////////////////////////////////////////////////
namespace puntoexe
{

namespace imebra
{

class lut;

namespace handlers
{
	/// \brief Represents an handler specialized to work with
	///         the image data.
	///
	/// The image data is always interleaved and is never
	///  subsampled.
	///
	/// The application never needs to resize the data managed
	///  by this handler because the object image does this.
	///
	///////////////////////////////////////////////////////////
	typedef dataHandlerNumeric<imbxInt32> imageHandler;
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief Represents a single image of a dicom data set.
///
/// Images are embedded into the dicom structures
///  (represented by the dataSet class), stored in
///  a compressed format.
///
/// The class image represents a decompressed raw image,
///  extracted from a dicom structure using
///  dataSet::getImage().
///
/// image objects can also be created by the
///  application and stored into a dicom structure using
///  the function dataSet::setImage().
///
/// The image and its buffer share a common lock object:
///  this means that a lock to the image object will also
///  locks the image's buffer and viceversa.
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class image : public baseObject
{
public:

	///////////////////////////////////////////////////////////
	///
	/// \brief Define a single color component's size.
	///
	///////////////////////////////////////////////////////////
	enum bitDepth
	{
		depthUnknown,   ///< the bit depth is unknown
			depthU8,    ///< unsigned integer, 1 byte
			depthS8,    ///< signed integer, 1 byte
			depthU16,   ///< unsigned integer, 2 bytes
			depthS16    ///< signed integer, 2 bytes
	};

	// Constructor
	///////////////////////////////////////////////////////////
	image():
		baseObject(ptr<baseObject>(new baseObject) ),
			m_rowLength(0),
			m_channelPixelSize(0),
			m_channelsNumber(0),
			m_imageDepth(depthUnknown),
			m_highBit(0),
			m_sizeX(0),
			m_sizeY(0),
			m_sizeMmX(0),
			m_sizeMmY(0){}


	/// \brief Create the image.
	///
	/// An image with the specified size (in pixels), bit depth
	///  and color space is allocated.
	/// The number of channels to allocate is automatically
	///  calculated using the colorSpace parameter.
	///
	/// @param sizeX    the image's width, in pixels.
	/// @param sizeY    the image's height, in pixels.
	/// @param depth    the size of a single color's component.
	/// @param colorSpace The color space as defined by the
	///                 DICOM standard.
	///                 Valid colorspace are:
	///                 - "RGB"
	///                 - "YBR_FULL"
	///                 - "YBR_PARTIAL"
	///                 - "YBR_RCT"
	///                 - "YBR_ICT"
	///                 - "PALETTE COLOR"
	///                 - "MONOCHROME2"
	///                 - "MONOCHROME1"
	/// @param highBit  the highest bit used for integer
	///                  values.
	/// @return         the data handler containing the image's
	///                  data
	///
	///////////////////////////////////////////////////////////
	ptr<handlers::imageHandler> create(
		const imbxUint32 sizeX,
		const imbxUint32 sizeY,
		const bitDepth depth,
		std::wstring colorSpace,
		const imbxUint8  highBit);

	/// \internal
	/// \brief Set the bit depth and the high bit.
	///
	/// This function is used by the transforms to set the
	///  bit depth and the high bit AFTER the image has been
	///  created and filled with the information.
	///
	/// @param highBit       the image's high bit
	/// @param imageBitDepth the image's bit depth
	///
	///////////////////////////////////////////////////////////
	void setBitDepthAndHighBit(imbxUint32 highBit, bitDepth imageBitDepth);

	/// \brief Retrieve the image's size, in millimeters.
	///
	/// The image's size in millimeters is automatically read
	///  from the dicom structure or can be set using
	///  setSizeMm().
	///
	/// @param pSizeX a pointer to the variable to fill with
	///               the image's width (in millimeters).
	/// @param pSizeY a pointer to the variable to fill with
	///               the image's height (in millimeters).
	///////////////////////////////////////////////////////////
	void getSizeMm(double* pSizeX, double* pSizeY);

	/// \brief Set the image's size, in millimeters.
	///
	/// @param sizeX the new image's width, in millimeters.
	/// @param sizeY the new image's height, in millimeters.
	///
	///////////////////////////////////////////////////////////
	void setSizeMm(const double sizeX, const double sizeY);

	/// \brief Get the image's size, in pixels.
	///
	/// @param pSizeX a pointer to the variable to fill with
	///               the image's width (in pixels).
	/// @param pSizeY a pointer to the variable to fill with
	///               the image's height (in pixels).
	///
	///////////////////////////////////////////////////////////
	void getSize(imbxUint32* pSizeX, imbxUint32* pSizeY);

	/// \brief Retrieve a data handler for managing the
	///        image's buffer
	///
	/// The retrieved data handler gives access to the image's
	///  buffer.
	/// The image's buffer stores the data in the following
	///  format:
	/// - when multiple channels are present, then the channels
	///   are ALWAYS interleaved
	/// - the channels are NEVER subsampled or oversampled.
	///   The subsampling/oversampling is done by the codecs
	///   when the image is stored or loaded from the dicom
	///   structure.
	/// - the first stored value represent the first channel of
	///   the top/left pixel.
	/// - each row is stored countiguously, from the top to the
	///   bottom.
	///
	/// @param bWrite   true if the application wants to write
	///                 into the buffer, false otherwise.
	/// @param pRowSize the function will fill the variable
	///                 pointed by this parameter with
	///                 the size of a single row, in bytes.
	/// @param pChannelPixelSize the function will fill the
	///                 variable pointed by this parameter with
	///                 the size of a single pixel,
	///                 in bytes.
	/// @param pChannelsNumber  the function will fill the
	///                 variable pointed by this parameter with
	///                 the number of channels per pixel.
	/// @return a pointer to the data handler for the image's
	///         buffer.
	///
	///////////////////////////////////////////////////////////
	ptr<handlers::imageHandler> getDataHandler(
		const bool bWrite,
		imbxUint32* pRowSize,
		imbxUint32* pChannelPixelSize,
		imbxUint32* pChannelsNumber);

	/// \brief Get the image's color space (DICOM standard)
	///
	/// @return a string with the image's color space
	///
	///////////////////////////////////////////////////////////
	std::wstring getColorSpace();

	/// \brief Get the number of allocated channels.
	///
	/// @return the number of color channels in the image
	///
	///////////////////////////////////////////////////////////
	imbxUint32 getChannelsNumber();

	/// \brief Get the image's bit depth.
	///
	/// The bit depth indicates how every single value is
	///  stored into the image's buffer.
	///
	/// @return the bit depth.
	///////////////////////////////////////////////////////////
	bitDepth getDepth();

	/// \brief Get the high bit.
	///
	/// @return the image's high bit
	///
	///////////////////////////////////////////////////////////
	imbxUint32 getHighBit();

protected:
	// Image's buffer
	///////////////////////////////////////////////////////////
	ptr<buffer> m_buffer;

	// Lenght of a buffer's row (in bytes)
	///////////////////////////////////////////////////////////
	imbxUint32 m_rowLength;

	// Length of a pixel's component (in bytes)
	///////////////////////////////////////////////////////////
	imbxUint32 m_channelPixelSize;

	// Number of channels
	///////////////////////////////////////////////////////////
	imbxUint32  m_channelsNumber;

	// Color space
	///////////////////////////////////////////////////////////
	std::wstring m_colorSpace;

	// Depth (enum)
	///////////////////////////////////////////////////////////
	bitDepth  m_imageDepth;

	// High bit (not valid in float mode)
	///////////////////////////////////////////////////////////
	imbxUint32 m_highBit;

	// Image's size in pixels
	///////////////////////////////////////////////////////////
	imbxUint32 m_sizeX;
	imbxUint32 m_sizeY;

	// Image's size in millimeters
	///////////////////////////////////////////////////////////
	double m_sizeMmX;
	double m_sizeMmY;
};


///////////////////////////////////////////////////////////
/// \brief This is the base class for the exceptions thrown
///         by the image class.
///
///////////////////////////////////////////////////////////
class imageException: public std::runtime_error
{
public:
	/// \brief Build a codec exception
	///
	/// @param message the message to store into the exception
	///
	///////////////////////////////////////////////////////////
	imageException(const std::string& message): std::runtime_error(message){}
};

///////////////////////////////////////////////////////////
/// \brief This exception is thrown when an unknown depth
///         is specified as a parameter.
///
///////////////////////////////////////////////////////////
class imageExceptionUnknownDepth: public imageException
{
public:
	imageExceptionUnknownDepth(const std::string& message): imageException(message){}
};

///////////////////////////////////////////////////////////
/// \brief This exception is thrown when an unknown color
///         space is specified in the function create().
///
///////////////////////////////////////////////////////////
class imageExceptionUnknownColorSpace: public imageException
{
public:
	imageExceptionUnknownColorSpace(const std::string& message): imageException(message){}
};

///////////////////////////////////////////////////////////
/// \brief This exception is thrown when an invalid size
///         in pixels is specified in the function
///         create().
///
///////////////////////////////////////////////////////////
class imageExceptionInvalidSize: public imageException
{
public:
	imageExceptionInvalidSize(const std::string& message): imageException(message){}
};


} // namespace imebra

} // namespace puntoexe

#endif // !defined(imebraImage_A807A3CA_FA04_44f4_85D2_C7AA2FE103C4__INCLUDED_)
