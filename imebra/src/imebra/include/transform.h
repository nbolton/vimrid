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

/*! \file transform.h
    \brief Declaration of the base class used by all the transforms.
 
*/

#if !defined(imebraTransform_5DB89BFD_F105_45e7_B9D9_3756AC93C821__INCLUDED_)
#define imebraTransform_5DB89BFD_F105_45e7_B9D9_3756AC93C821__INCLUDED_

#include "../../base/include/baseObject.h"
#include "dataSet.h"

#include <map>


///////////////////////////////////////////////////////////
//
// Everything is in the namespace puntoexe::imebra
//
///////////////////////////////////////////////////////////
namespace puntoexe
{

namespace imebra
{

class image;

/// \namespace puntoexe::imebra::transforms
/// \brief This namespace contains all the transforms
///         of the imebra library.
///
/// A transform is an object that takes some images as
///  input parameters and returns some images as a
///  result of a transformation or analysis.
///
///////////////////////////////////////////////////////////
namespace transforms
{

///////////////////////////////////////////////////////////
/// \brief This is the base class for the transforms.
///
/// A transform can process several input images and
///  reply with several output images.
///
/// Sometimes the transform needs to know the dataSet
///  related to the images being processed; in this case
///  the function transform::declareDataSet() must be
///  called.
///
///////////////////////////////////////////////////////////
class transform : public baseObject
{

public:
	/// \brief Declare an input image for the transform
	///
	/// The transform will use the input image for its
	///  purposes when the application will call 
	///  doTransform().
	///
	/// @param imageNumber the transform can work with more
	///                     than one input image.
	///                    This parameter specifies the
	///                     parameter's ID.
	///                    If another image was already stored
	///                     with this parameter's ID, then it
	///                     will be released.
	/// @param pInputImage the image to use as the transform's 
	///                     input parameter
	///
	///////////////////////////////////////////////////////////
	virtual void declareInputImage(long imageNumber, ptr<image> pInputImage);

	/// \brief Declare an output image for the transform
	///
	/// The transform will store the results in the output
	///  image.
	///
	/// @param imageNumber the transform can work with more
	///                     than one output image.
	///                    This parameter specifies the
	///                     parameter's ID.
	///                    If another image was already stored
	///                     with this parameter's ID, then it
	///                     will be released
	/// @param pOutputImage the image to use as the transform's 
	///                     output parameter
	///
	///////////////////////////////////////////////////////////
	virtual void declareOutputImage(long imageNumber, ptr<image> pOutputImage);
	
	/// \brief Retrieve an image used as input parameter.
	///
	/// @param imageNumber the ID of the input parameter to
	///                     retrieve
	/// @return            the image used as input parameter
	///                     or 0 if the input parameter has
	///                     not been defined
	///
	///////////////////////////////////////////////////////////
	ptr<image> getOutputImage(long imageNumber);

	/// \brief Retrieve an image used as output parameter.
	///
	/// @param imageNumber the ID of the output parameter to
	///                     retrieve.
	/// @return            the image used as output parameter
	///                     or 0 if the output parameter has
	///                     not been defined
	///
	///////////////////////////////////////////////////////////
	ptr<image> getInputImage(long imageNumber);

	/// \brief Set the dataset to use for the transformations
	///
	/// Some transformations need additional data from a
	///  dataset.
	/// E.G.: the modalityVOILUT transform needs to load
	///       the LUT table or the rescale/intercept values
	///       from the dataset.
	///
	/// @param pDataSet the dataset to use for the 
	///                  transformation
	///                  
	///////////////////////////////////////////////////////////
	virtual void declareDataSet(ptr<dataSet> pDataSet);
	
	/// \brief Retrieve the dataset used for the
	///        transformation.
	///
	/// The dataSet can be set by calling declareDataSet().
	///
	/// @return a pointer to the active data set, or 0 if
	///          the dataset has not been set
	///////////////////////////////////////////////////////////
	ptr<dataSet> getDataSet();

	/// \brief Executes the transform.
	///
	///////////////////////////////////////////////////////////
	virtual void doTransform()=0;

	/// \brief Returns true if the transform doesn't do
	///         anything.
	///
	/// It always return false, but it is overwritten in the
	///  transformsChain class.
	///
	/// @return false if the transform does something, or true
	///          if the transform doesn't do anything (e.g. an
	///          empty transformsChain object).
	///
	///////////////////////////////////////////////////////////
	virtual bool isEmpty();

private:
	typedef std::map<int, ptr<image> > tImagesMap;
	tImagesMap m_inputImages;
	tImagesMap m_outputImages;

	ptr<dataSet> m_pDataSet;
};


///////////////////////////////////////////////////////////
/// \brief This is the base class of the exceptions thrown
///         by the classes derived from transform.
///
///////////////////////////////////////////////////////////
class transformException: public std::runtime_error
{
public:
	transformException(const std::string& message): std::runtime_error(message){}
};


///////////////////////////////////////////////////////////
/// \brief This exception is thrown by the transforms that
///         need a defined dataSet (see declareDataSet())
///         but don't have any.
///
///////////////////////////////////////////////////////////
class transformExceptionDataSetNotDefined: public transformException
{
public:
	transformExceptionDataSetNotDefined(const std::string& message): transformException(message){}
};


///////////////////////////////////////////////////////////
/// \brief This exception is thrown by a transforms when an
///         input image is required but hasn't been defined
///         (see declareInputImage()).
///
///////////////////////////////////////////////////////////
class transformExceptionDataInputImageDefined: public transformException
{
public:
	transformExceptionDataInputImageDefined(const std::string& message): transformException(message){}
};


///////////////////////////////////////////////////////////
/// \brief This exception is thrown by a transforms when an
///         output image is required but hasn't been 
///         defined (see declareOutputImage()).
///
///////////////////////////////////////////////////////////
class transformExceptionOutputImageNotDefined: public transformException
{
public:
	transformExceptionOutputImageNotDefined(const std::string& message): transformException(message){}
};


} // namespace transforms

} // namespace imebra

} // namespace puntoexe

#endif // !defined(imebraTransform_5DB89BFD_F105_45e7_B9D9_3756AC93C821__INCLUDED_)
