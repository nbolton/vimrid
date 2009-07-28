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

/*! \file transformBuffers.h
    \brief Declaration of the class transformBuffers.

*/

#if !defined(imebraTransformBuffers_5DB89BFD_F105_45e7_B9D9_3756AC93C821__INCLUDED_)
#define imebraTransformBuffers_5DB89BFD_F105_45e7_B9D9_3756AC93C821__INCLUDED_

#include "transform.h"
#include "image.h"


///////////////////////////////////////////////////////////
//
// Everything is in the namespace puntoexe::imebra
//
///////////////////////////////////////////////////////////
namespace puntoexe
{

namespace imebra
{

namespace transforms
{

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief This class is used as a base class by some
///         transforms that work on the image's pixels.
///
/// Don't use this class directly but allocate
///  CTransformBuffersInputOutput or
///  CTransformBuffersInPlace.
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class transformBuffers : public transform
{

public:
    void doTransform();

protected:
    /// \brief Get the data buffers and calls the
    ///         doTransformBuffers().
    ///
    /// Retrieve the data buffers from the input and output
    ///  images and call the derived class processing
    ///  function.
    ///
    /// @param inputImage  the input image
    /// @param outputImage the output image
    /// @param sizeX       the buffer's width, in pixels
    /// @param sizeY       the buffer's height, in pixels
    /// @param inputChannelsNumber the number of channels in
    ///                     the input image
    /// @param inputColorSpace the color space of the input
    ///                     image
    /// @param inputDepth  the bit depth of the input image
    /// @param inputHighBit the input image's high bit
    /// @param pOutputDepth pointer to the variable that
    ///                     the function fills with the output
    ///                     image's bit depth
    /// @param pOutputHighBit pointer to the variable that
    ///                     the function fills with the output
    ///                     image's high bit
    ///
    ///////////////////////////////////////////////////////////
    virtual void processDataBuffers(ptr<image> inputImage, ptr<image> outputImage,
		imbxUint32 sizeX,
		imbxUint32 sizeY,
		std::wstring inputColorSpace,
		image::bitDepth inputDepth,
		imbxUint32 inputHighBit,
		image::bitDepth* pOutputDepth,
		imbxUint32* pOutputHighBit) = 0;
};


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief This class is used as a base class by some
///         transforms that work on the image's pixels and
///         need two different buffers (one for the input
///         image and one for the outpu image).
///
/// The class takes the input image and automatically
///  allocate an output image if it hasn't been declared.
///
/// Then the virtual function
///  doTransformBuffersInputOutput() is called.
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class transformBuffersInputOutput : public transformBuffers
{

protected:
	virtual void doTransformBuffersInputOutput(
		imbxUint32 sizeX,
		imbxUint32 sizeY,
		imbxUint32 inputChannelsNumber,
		std::wstring inputColorSpace,
		image::bitDepth inputDepth,
		imbxUint32 inputHighBit,
		imbxInt32* pInputBuffer,
		imbxInt32* pOutputBuffer,
		imbxUint32 buffersSize,
		image::bitDepth* pOutputDepth,
		imbxUint32* pOutputHighBit
		)=0;

    virtual void processDataBuffers(ptr<image> inputImage, ptr<image> outputImage,
		imbxUint32 sizeX,
		imbxUint32 sizeY,
		std::wstring inputColorSpace,
		image::bitDepth inputDepth,
		imbxUint32 inputHighBit,
		image::bitDepth* pOutputDepth,
		imbxUint32* pOutputHighBit);
};


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief This class is used as a base class by some
///         transforms that work on the image's pixels and
///         don't need two different buffers (input &
///         output) but can work on a single buffer that
///         contains the input and output data.
///
/// The class takes the input image and automatically
///  allocate an output image if it hasn't been declared.
///
/// Then the virtual function doTransformBuffersInPlace()
///  is called.
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class transformBuffersInPlace : public transformBuffers
{

protected:
	virtual void doTransformBuffersInPlace(
		imbxUint32 sizeX,
		imbxUint32 sizeY,
		imbxUint32 inputChannelsNumber,
		std::wstring inputColorSpace,
		image::bitDepth inputDepth,
		imbxUint32 inputHighBit,
		imbxInt32* pBuffer,
		imbxUint32 buffersSize,
		image::bitDepth* pOutputDepth,
		imbxUint32* pOutputHighBit
		)=0;

    virtual void processDataBuffers(ptr<image> inputImage, ptr<image> outputImage,
		imbxUint32 sizeX,
		imbxUint32 sizeY,
		std::wstring inputColorSpace,
		image::bitDepth inputDepth,
		imbxUint32 inputHighBit,
		image::bitDepth* pOutputDepth,
		imbxUint32* pOutputHighBit);
};



} // namespace transforms

} // namespace imebra

} // namespace puntoexe

#endif // !defined(imebraTransformBuffers_5DB89BFD_F105_45e7_B9D9_3756AC93C821__INCLUDED_)
