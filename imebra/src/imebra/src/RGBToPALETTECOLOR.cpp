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

/*! \file RGBToPALETTECOLOR.cpp
    \brief Implementation of the class RGBToPALETTECOLOR.

*/

#include "../../base/include/exception.h"
#include "../include/RGBToPALETTECOLOR.h"
#include "../include/dataSet.h"
#include "../include/dataHandler.h"
#include "../include/image.h"

namespace puntoexe
{

namespace imebra
{

namespace transforms
{

namespace colorTransforms
{

static registerColorTransform m_registerTransform(ptr<colorTransform>(new RGBToPALETTECOLOR));

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Return the initial color space
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::wstring RGBToPALETTECOLOR::getInitialColorSpace()
{
	return L"RGB";
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Return the final color space
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::wstring RGBToPALETTECOLOR::getFinalColorSpace()
{
	return L"PALETTE COLOR";
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Create a new RGBToPALETTECOLOR transform
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
ptr<colorTransform> RGBToPALETTECOLOR::createColorTransform()
{
	return ptr<colorTransform>(new RGBToPALETTECOLOR);
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Declare the dataset
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void RGBToPALETTECOLOR::declareDataSet(ptr<dataSet> pDataSet)
{
	PUNTOEXE_FUNCTION_START(L"RGBToPALETTECOLOR::declareDataSet");

	PALETTECOLORToRGB::declareDataSet(pDataSet);

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Declare the lut to use for the conversion
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void RGBToPALETTECOLOR::setLut(ptr<lut> red, ptr<lut> green, ptr<lut> blue)
{
	PUNTOEXE_FUNCTION_START(L"RGBToPALETTECOLOR::setLut");

	PALETTECOLORToRGB::setLut(red, green, blue);
	resetOctree(m_redPalette->getBits());

	imbxInt32 paletteSize = (imbxInt32)(m_redPalette->getSize());
	imbxInt32 firstMapped = m_redPalette->getFirstMapped();

	for(imbxInt32 scanPalette = firstMapped; scanPalette < paletteSize+firstMapped; ++scanPalette)
	{
		imbxInt32 red = m_redPalette->mappedValue(scanPalette);
		imbxInt32 green = m_greenPalette->mappedValue(scanPalette);
		imbxInt32 blue = m_bluePalette->mappedValue(scanPalette);
		insertOctreeColor(red, green, blue, scanPalette);
	}

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Convert the input images from RGB to PALETTE COLOR
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void RGBToPALETTECOLOR::doTransform()
{
	PUNTOEXE_FUNCTION_START(L"RGBToPALETTECOLOR::doTransform");

	// Get the source image's palette
	///////////////////////////////////////////////////////////
	lut* pRed = m_redPalette.get();
	lut* pGreen = m_greenPalette.get();
	lut* pBlue = m_bluePalette.get();

	if(pRed == 0 || pGreen == 0 || pBlue == 0)
	{
		findPalette();
	}

	// Convert all the input images
	///////////////////////////////////////////////////////////
	for(int scanImages = 0; ; ++scanImages)
	{
		// Get the input image
		///////////////////////////////////////////////////////////
		ptr<image> pInputImage=getInputImage(scanImages);

		// If the input image doesn't exist, then exit
		///////////////////////////////////////////////////////////
		if(pInputImage == 0)
			break;

		// Check the input color space
		///////////////////////////////////////////////////////////
		if(pInputImage->getColorSpace()!=getInitialColorSpace())
			break;

		// Get the output image
		///////////////////////////////////////////////////////////
		ptr<image> pOutputImage=getOutputImage(scanImages);
		if(pOutputImage == 0)
		{
			ptr<image> tempImage(new image);
			pOutputImage=tempImage;
			declareOutputImage(scanImages, pOutputImage);
		}

		// Get the input image's attributes and the data handler
		///////////////////////////////////////////////////////////
		imbxUint32 sizeX, sizeY;
		pInputImage->getSize(&sizeX, &sizeY);

		double sizeMmX, sizeMmY;
		pInputImage->getSizeMm(&sizeMmX, &sizeMmY);
		pOutputImage->setSizeMm(sizeMmX, sizeMmY);

		image::bitDepth depth = image::depthU8;
		if(m_octreeBits > 8)
		{
			depth = image::depthU16;
		}

		imbxUint32 totalPixelsNumber=sizeX*sizeY;

		// Create the output image
		///////////////////////////////////////////////////////////
		ptr<handlers::imageHandler> pOutputDataHandler = pOutputImage->create(sizeX, sizeY, depth, getFinalColorSpace(), m_octreeBits - 1);

		// Get the data handler for the input and the output
		//  images
		///////////////////////////////////////////////////////////
		imbxUint32 rowSize, channelPixelSize, channelsNumber;
		ptr<handlers::imageHandler> pInputDataHandler=pInputImage->getDataHandler(false, &rowSize, &channelPixelSize, &channelsNumber);

		imbxInt32* pSourceBuffer = pInputDataHandler->getMemoryBuffer();
		imbxInt32* pDestBuffer = pOutputDataHandler->getMemoryBuffer();

		imbxInt32 red, green, blue;
		while(totalPixelsNumber--)
		{
			red = *pSourceBuffer++;
			green = *pSourceBuffer++;
			blue = *pSourceBuffer++;

			*pDestBuffer++ = getOctreeIndex(red, green, blue);
		}
	}
	
	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Find the best palette that can be used to convert
//  the input images from RGB to PALETTE COLOR
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void RGBToPALETTECOLOR::findPalette()
{
	PUNTOEXE_FUNCTION_START(L"RGBToPALETTECOLOR::findPalette");

	bool bResetOctree = true;

	// Process all the input images
	///////////////////////////////////////////////////////////
	for(int scanImages = 0; ; ++scanImages)
	{
		// Get the input image
		///////////////////////////////////////////////////////////
		ptr<image> pInputImage=getInputImage(scanImages);

		if(bResetOctree)
		{
			resetOctree((char)(pInputImage->getHighBit() + 1));
			bResetOctree = false;
		}

		// If the input image doesn't exist, then exit
		///////////////////////////////////////////////////////////
		if(pInputImage == 0)
			break;

		// Check the input color space
		///////////////////////////////////////////////////////////
		if(pInputImage->getColorSpace()!=getInitialColorSpace())
			break;

		// Get the input image's attributes and the data handler
		///////////////////////////////////////////////////////////
		imbxUint32 sizeX, sizeY;
		pInputImage->getSize(&sizeX, &sizeY);
		imbxUint32 totalPixelsNumber=sizeX*sizeY;

		// Get the data handler for the input and the output
		//  images
		///////////////////////////////////////////////////////////
		imbxUint32 rowSize, channelPixelSize, channelsNumber;
		ptr<handlers::imageHandler> pInputDataHandler=pInputImage->getDataHandler(false, &rowSize, &channelPixelSize, &channelsNumber);
		imbxInt32* pSourceBuffer = pInputDataHandler->getMemoryBuffer();

		imbxInt32 red, green, blue;
		while(totalPixelsNumber--)
		{
			red = *pSourceBuffer++;
			green = *pSourceBuffer++;
			blue = *pSourceBuffer++;
			insertOctreeColor(red, green, blue, 0);
		}
	}

	reduceOctreeColor((imbxUint32)0x1 << m_octreeBits);

	generatePaletteFromOctree();

	ptr<dataSet> pDataSet = getDataSet();
	if(pDataSet == 0)
	{
		return;
	}
	
	ptr<handlers::dataHandler> pRedDescriptor=pDataSet->getDataHandler(0x0028, 0x0, 0x1101, 0, true);
	ptr<handlers::dataHandler> pRedData=pDataSet->getDataHandler(0x0028, 0x0, 0x1201, 0, true);
	m_redPalette->fillHandlers(pRedDescriptor, pRedData);

	ptr<handlers::dataHandler> pGreenDescriptor=pDataSet->getDataHandler(0x0028, 0x0, 0x1102, 0, true);
	ptr<handlers::dataHandler> pGreenData=pDataSet->getDataHandler(0x0028, 0x0, 0x1202, 0, true);
	m_greenPalette->fillHandlers(pGreenDescriptor, pGreenData);

	ptr<handlers::dataHandler> pBlueDescriptor=pDataSet->getDataHandler(0x0028, 0x0, 0x1103, 0, true);
	ptr<handlers::dataHandler> pBlueData=pDataSet->getDataHandler(0x0028, 0x0, 0x1203, 0, true);
	m_bluePalette->fillHandlers(pBlueDescriptor, pBlueData);

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Don't call this function directly
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void RGBToPALETTECOLOR::doColorTransform(imbxInt32* pSourceMem, imbxInt32* pDestMem, imbxUint32 pixelsNumber, imbxInt32 inputMinValue, imbxInt32 inputMaxValue, imbxInt32 outputMinValue, imbxInt32 outputMaxValue)
{
	PUNTOEXE_FUNCTION_START(L"RGBToPALETTECOLOR::doColorTransform");

	PALETTECOLORToRGB::doColorTransform(pSourceMem, pDestMem, pixelsNumber, inputMinValue, inputMaxValue, outputMinValue, outputMaxValue);

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Reset the octree structure
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void RGBToPALETTECOLOR::resetOctree(char bits)
{
	for(unsigned char scanChildren = 0; scanChildren < 8; ++scanChildren)
	{
		m_rootNode.m_children[scanChildren].release();
	}
	m_leavesCount = 0;
	m_octreeBits = bits;
	m_octreeMask = (imbxInt32)0x1 << (bits - 1);
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Insert a color in the octree structure
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void RGBToPALETTECOLOR::insertOctreeColor(imbxInt32 red, imbxInt32 green, imbxUint32 blue, imbxInt32 index)
{
	octreeNode* pNode = &m_rootNode;
	imbxInt32 origRed = red;
	imbxInt32 origGreen = green;
	imbxInt32 origBlue = blue;
	for(char level = m_octreeBits; level != 0; --level)
	{
		char octreeIndex = (char)(((red & m_octreeMask) >> (m_octreeBits - 3)) | ((green & m_octreeMask) >> (m_octreeBits - 2))  | ((blue & m_octreeMask) >> (m_octreeBits - 1)));
		red <<= 1;
		green <<= 1;
		blue <<= 1;

		octreeNode* pChildNode = pNode->m_children[octreeIndex].get();
		if(pChildNode == 0)
		{
			if(pNode->m_childrenCount++ != 0)
			{
				++m_leavesCount;
			}

			pChildNode = new octreeNode;
			pNode->m_children[octreeIndex].reset(pChildNode);
		}

		++(pChildNode->m_reference);
		pChildNode->m_red += origRed;
		pChildNode->m_green += origGreen;
		pChildNode->m_blue += origBlue;
		if(pChildNode->m_index == 0)
		{
			pChildNode->m_index = index;
		}

		pNode = pChildNode;
	}
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Return the index for the specified color
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxUint32 RGBToPALETTECOLOR::getOctreeIndex(imbxInt32 red, imbxInt32 green, imbxInt32 blue)
{
	octreeNode* pNode = &m_rootNode;
	for(char level = m_octreeBits; level != 0; --level)
	{
		char index = (char)( ((red & m_octreeMask) >> (m_octreeBits - 3)) | ((green & m_octreeMask) >> (m_octreeBits - 2))  | ((blue & m_octreeMask) >> (m_octreeBits - 1)) );
		red <<= 1;
		green <<= 1;
		blue <<= 1;

		octreeNode* pChildNode = pNode->m_children[index].get();
		if(pChildNode == 0)
		{
			return pNode->m_index;
		}

		pNode = pChildNode;
	}
	return 0;
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Reduce the number of leaf nodes in the octree structure
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void RGBToPALETTECOLOR::reduceOctreeColor(imbxUint32 size)
{
	PUNTOEXE_FUNCTION_START(L"RGBToPALETTECOLOR::reduceOctreeColor");

	while(m_leavesCount > size)
	{
		imbxUint32 minReferenceCount = 0;
		octreeNode* pNode = 0;
		m_rootNode.findNodeToReduce(&minReferenceCount, &pNode);
		if(pNode == 0 || pNode == &m_rootNode)
		{
			break;
		}
		for(char resetChildren = 0; resetChildren < 8; ++resetChildren)
		{
			pNode->m_children[resetChildren].release();
		}
		m_leavesCount -= pNode->m_childrenCount - 1;
		pNode->m_childrenCount = 0;
	}

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Find the octree node that can be reduced
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void RGBToPALETTECOLOR::octreeNode::findNodeToReduce(imbxUint32* pMinReferenceCount, octreeNode** pReduceNode)
{
	if(m_childrenCount == 0)
	{
		return;
	}

	bool bCanBeReduced = true;
	for(int children = 0; children < 8; ++children)
	{
		octreeNode* pChild = m_children[children].get();
		if(pChild == 0)
		{
			continue;
		}
		if(pChild->m_childrenCount != 0)
		{
			pChild->findNodeToReduce(pMinReferenceCount, pReduceNode);
			bCanBeReduced = false;
			continue;
		}
	}
	if(bCanBeReduced && (*pMinReferenceCount == 0 || m_reference < *pMinReferenceCount))
	{
		*pMinReferenceCount = m_reference;
		*pReduceNode = this;
	}
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Copy the colors from the octree structure into the
//  palette
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void RGBToPALETTECOLOR::octreeNode::assignColors(lut* pRed, lut* pGreen, lut* pBlue, imbxUint32* pIndex)
{
	if(m_childrenCount == 0)
	{
		pRed->setLutValue(*pIndex, m_red / m_reference);
		pGreen->setLutValue(*pIndex, m_green / m_reference);
		pBlue->setLutValue(*pIndex, m_blue / m_reference);
		m_index = *pIndex;
		++(*pIndex);
		return;
	}
	for(char scanIndex = 0; scanIndex < 8; ++scanIndex)
	{
		octreeNode* pNode = m_children[scanIndex].get();
		if(pNode != 0)
		{
			pNode->assignColors(pRed, pGreen, pBlue, pIndex);
		}
	}
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Generate a palette from the octree structure
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void RGBToPALETTECOLOR::generatePaletteFromOctree()
{
	PUNTOEXE_FUNCTION_START(L"RGBToPALETTECOLOR::generatePaletteFromOctree");

	ptr<lut> lutRed(new lut);
	ptr<lut> lutGreen(new lut);
	ptr<lut> lutBlue(new lut);

	lutRed->create(m_leavesCount, 0, 8, L"Red component");
	lutGreen->create(m_leavesCount, 0, 8, L"Green component");
	lutBlue->create(m_leavesCount, 0, 8, L"Blue component");

	imbxUint32 index = 0;
	m_rootNode.assignColors(lutRed.get(), lutGreen.get(), lutBlue.get(), &index);

	m_redPalette = lutRed;
	m_greenPalette = lutGreen;
	m_bluePalette = lutBlue;

	PUNTOEXE_FUNCTION_END();
}


} // namespace colorTransforms

} // namespace transforms

} // namespace imebra

} // namespace puntoexe


