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

/*! \file transformsChain.cpp
    \brief Implementation of the class transformsChain.

*/

#include "../../base/include/exception.h"
#include "../include/transformsChain.h"
#include "../include/image.h"
#include "../include/dataSet.h"

namespace puntoexe
{

namespace imebra
{

namespace transforms
{

///////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////
transformsChain::transformsChain()
{
	m_bEndTransformsChainCalled = false;
}

///////////////////////////////////////////////////////////
//
// Declare an input image
//
///////////////////////////////////////////////////////////
void transformsChain::declareInputImage(long imageNumber, ptr<image> pInputImage)
{
	PUNTOEXE_FUNCTION_START(L"transformsChain::declareInputImage");

	transform::declareInputImage(imageNumber, pInputImage);
	if(m_transformsList.size() != 0)
	{
		m_transformsList.front()->declareInputImage(imageNumber, pInputImage);
	}

	PUNTOEXE_FUNCTION_END();
}

///////////////////////////////////////////////////////////
//
// Declare an output image
//
///////////////////////////////////////////////////////////
void transformsChain::declareOutputImage(long imageNumber, ptr<image> pOutputImage)
{
	PUNTOEXE_FUNCTION_START(L"transformsChain::declareOutputImage");

	transform::declareOutputImage(imageNumber, pOutputImage);
	if(m_bEndTransformsChainCalled && m_transformsList.size() != 0)
	{
		m_transformsList.back()->declareOutputImage(imageNumber, pOutputImage);
	}

	PUNTOEXE_FUNCTION_END();
}

///////////////////////////////////////////////////////////
//
// Set the dataset to use for the transformations
//
///////////////////////////////////////////////////////////
void transformsChain::declareDataSet(ptr<dataSet> pDataSet)
{
	PUNTOEXE_FUNCTION_START(L"transformsChain::declareDataSet");

	transform::declareDataSet(pDataSet);

	// All the transforms in the chain will use the dataset
	///////////////////////////////////////////////////////////
	for(tTransformsList::iterator scanTransforms = m_transformsList.begin(); scanTransforms != m_transformsList.end(); ++scanTransforms)
	{
		(*scanTransforms)->declareDataSet(pDataSet);
	}

	PUNTOEXE_FUNCTION_END();
}

///////////////////////////////////////////////////////////
//
// Add a new transform to the chain
//
///////////////////////////////////////////////////////////
void transformsChain::addTransform(ptr<transform> pTransform)
{
	PUNTOEXE_FUNCTION_START(L"transformsChain::addTransform");

	// If the function endTransformsChain has already been
	//  called then reset the chain and restart
	///////////////////////////////////////////////////////////
	if(m_bEndTransformsChainCalled)
	{
		m_transformsList.clear();
		m_bEndTransformsChainCalled = false;
	}

	// This is the first transform. Copy the input images
	//  into it.
	///////////////////////////////////////////////////////////
	if(m_transformsList.size() == 0)
	{
		for(long copyInputImages = 0; ; ++copyInputImages)
		{
			ptr<image> inputImage = getInputImage(copyInputImages);
			if(inputImage == 0)
			{
				break;
			}
			pTransform->declareInputImage(copyInputImages, inputImage);
		}
	}

	// The transform will use the same dataset defined for
	//  the chain.
	///////////////////////////////////////////////////////////
	ptr<dataSet> pDataSet = getDataSet();
	if(pDataSet != 0)
	{
		pTransform->declareDataSet(pDataSet);
	}

	// Store the transform in the chain.
	///////////////////////////////////////////////////////////
	m_transformsList.push_back(pTransform);

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
//
// Returns true if no transform has been defined
//
///////////////////////////////////////////////////////////
bool transformsChain::isEmpty()
{
	return m_transformsList.empty();
}


///////////////////////////////////////////////////////////
//
// Tells that no more transforms will be inserted in the
//  chain.
//
///////////////////////////////////////////////////////////
void transformsChain::endTransformsChain()
{
	PUNTOEXE_FUNCTION_START(L"transformsChain::endTransformsChain");

	// If this function has already been called, then return
	///////////////////////////////////////////////////////////
	if(m_bEndTransformsChainCalled || m_transformsList.size() == 0)
	{
		return;
	}

	// Remember we was called
	///////////////////////////////////////////////////////////
	m_bEndTransformsChainCalled = true;

	// Copy all the defined output images to the last transform
	//  in the chain.
	///////////////////////////////////////////////////////////
	for(long copyOutputImages = 0; ; ++copyOutputImages)
	{
		ptr<image> outputImage = getOutputImage(copyOutputImages);
		if(outputImage == 0)
		{
			break;
		}
		m_transformsList.back()->declareOutputImage(copyOutputImages, outputImage);
	}

	PUNTOEXE_FUNCTION_END();
}

///////////////////////////////////////////////////////////
//
// Execute all the transforms in the chain
//
///////////////////////////////////////////////////////////
void transformsChain::doTransform()
{
	PUNTOEXE_FUNCTION_START(L"transformsChain::doTransform");

	// Just to be sure...
	///////////////////////////////////////////////////////////
	endTransformsChain();

	// The last executed transform
	///////////////////////////////////////////////////////////
	transform* pPreviousTransform = 0;
	
	// Scan all the transforms
	///////////////////////////////////////////////////////////
	for(tTransformsList::iterator scanTransforms = m_transformsList.begin(); scanTransforms != m_transformsList.end(); ++scanTransforms)
	{
		// If we already execute one transform, then use its output
		//  as input for the next transform.
		///////////////////////////////////////////////////////////
		if(pPreviousTransform != 0)
		{
			for(long copyOutputImages = 0; ; ++copyOutputImages)
			{
				ptr<image> outputImage = pPreviousTransform->getOutputImage(copyOutputImages);
				if(outputImage == 0)
				{
					break;
				}
				(*scanTransforms)->declareInputImage(copyOutputImages, outputImage);
			}
		}
		
		// Execute the transform
		///////////////////////////////////////////////////////////
		(*scanTransforms)->doTransform();

		// Remember the last transform
		///////////////////////////////////////////////////////////
		pPreviousTransform = scanTransforms->get();
	}

	// Copy the results back to this object
	///////////////////////////////////////////////////////////
	if(pPreviousTransform != 0)
	{
		for(long copyOutputImages = 0; ; ++copyOutputImages)
		{
			ptr<image> outputImage = pPreviousTransform->getOutputImage(copyOutputImages);
			if(outputImage == 0)
			{
				break;
			}
			declareOutputImage(copyOutputImages, outputImage);
		}

		return;
	}

	// No transform performed.
	for(long copyInputImages = 0; ; ++copyInputImages)
	{
		ptr<image> inputImage = getOutputImage(copyInputImages);
		declareOutputImage(copyInputImages, inputImage);
		if(inputImage == 0)
		{
			break;
		}
	}

	return;

	PUNTOEXE_FUNCTION_END();
}


} // namespace transforms

} // namespace imebra

} // namespace puntoexe
