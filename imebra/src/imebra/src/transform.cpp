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

/*! \file transform.cpp
    \brief Implementation of the base class used by the transforms.

*/

#include "../../base/include/exception.h"
#include "../include/transform.h"
#include "../include/image.h"


namespace puntoexe
{

namespace imebra
{

namespace transforms
{

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Declare an input parameter
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void transform::declareInputImage(long imageNumber, ptr<image> pInputImage)
{
	PUNTOEXE_FUNCTION_START(L"transform::declareInputImage");

	lockObject lockAccess(this);

	// Add the new parameter
	///////////////////////////////////////////////////////////
	m_inputImages[imageNumber]=pInputImage;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Delare an output parameter
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void transform::declareOutputImage(long imageNumber, ptr<image> pOutputImage)
{
	PUNTOEXE_FUNCTION_START(L"transform::declareOutputImage");

	lockObject lockAccess(this);

	// Add the new parameter
	///////////////////////////////////////////////////////////
	m_outputImages[imageNumber]=pOutputImage;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Retrieve an output parameter
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
ptr<image> transform::getOutputImage(long imageNumber)
{
	PUNTOEXE_FUNCTION_START(L"transform::getOutputImage");

	lockObject lockAccess(this);

	tImagesMap::iterator findImage = m_outputImages.find(imageNumber);
	if(findImage != m_outputImages.end())
	{
		return findImage->second;
	}
	
	ptr<image> pImage;
	return pImage;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Retrieve an input parameter
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
ptr<image> transform::getInputImage(long imageNumber)
{
	PUNTOEXE_FUNCTION_START(L"transform::getInputImage");

	lockObject lockAccess(this);
	
	tImagesMap::iterator findImage = m_inputImages.find(imageNumber);
	if(findImage != m_inputImages.end())
	{
		return findImage->second;
	}

	ptr<image> pImage;
	return pImage;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Declare the dataset to use for the transformations.
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void transform::declareDataSet(ptr<dataSet> pDataSet)
{
	PUNTOEXE_FUNCTION_START(L"transform::declareDataSet");

	lockObject lockAccess(this);
	m_pDataSet=pDataSet;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Retrieve the active dataset
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
ptr<dataSet> transform::getDataSet()
{
	lockObject lockAccess(this);
	return m_pDataSet;
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Returns true if the transform doesn't do anything.
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
bool transform::isEmpty()
{
	return false;
}

} // namespace transforms

} // namespace imebra

} // namespace puntoexe
