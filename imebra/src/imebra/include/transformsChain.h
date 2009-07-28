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

/*! \file transformsChain.h
    \brief Declaration of the class transformsChain.

*/

#if !defined(imebraTransformsChain_5DB89BFD_F105_45e7_B9D9_3756AC93C821__INCLUDED_)
#define imebraTransformsChain_5DB89BFD_F105_45e7_B9D9_3756AC93C821__INCLUDED_

#include <list>
#include "transform.h"

namespace puntoexe
{

namespace imebra
{

namespace transforms
{


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief Executes a sequence of transforms.
///
/// Before calling doTransform, specify the sequence
///  by calling addTransform().
/// Each specified transforms take the output of the 
///  previous transform as input.
///
/// When all the transforms have been defined, call
///  endTransformsChain().
///
/// The first defined transform takes the input images
///  defined in the transformsChain object, the last
///  defined transforms uses the output images defined
///  in the transformsChain object.
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class transformsChain: public transform
{
public:
	transformsChain();

	// Declare an input image for the transform
	///////////////////////////////////////////////////////////
	virtual void declareInputImage(long imageNumber, ptr<image> pInputImage);

	// Declare an output image for the transform
	///////////////////////////////////////////////////////////
	virtual void declareOutputImage(long imageNumber, ptr<image> pOutputImage);

	// Set the dataset to use for the transformations
	///////////////////////////////////////////////////////////
	virtual void declareDataSet(ptr<dataSet> pDataSet);
	
	// Start the transform
	///////////////////////////////////////////////////////////
	virtual void doTransform();

	/// \brief Add a transform to the transforms chain.
	///
	/// The added transform will take the output of the 
	///  previously added transform as an input image and will
	///  supply its output to the next added transform or as
	///  an output of the transformsChain if it is the
	///  last added transform.
	///
	/// After all the transforms have been added the 
	///  application must call endTransformsChain() before
	///  calling doTransform().
	///
	/// @param pTransform the transform to be added to
	///                    transformsChain
	///
	///////////////////////////////////////////////////////////
	void addTransform(ptr<transform> pTransform);

	/// \brief Returns true if no transform has been defined
	///
	/// @return true if the transforms chain is empty and will
	///               not perform any transformation
	///
	///////////////////////////////////////////////////////////
	virtual bool isEmpty();

	/// \brief Tells to the transformsChain object that no more 
	///         transforms will be added to the chain.
	///
	/// This function MUST be called after all the transforms
	///  have been added to the chain by addTransform() and
	///  before calling doTransform().
	///
	///////////////////////////////////////////////////////////
	void endTransformsChain();

protected:
	typedef std::list<ptr<transform> > tTransformsList;
	tTransformsList m_transformsList;
	bool m_bEndTransformsChainCalled;
};


} // namespace transforms

} // namespace imebra

} // namespace puntoexe

#endif // !defined(imebraTransformsChain_5DB89BFD_F105_45e7_B9D9_3756AC93C821__INCLUDED_)
