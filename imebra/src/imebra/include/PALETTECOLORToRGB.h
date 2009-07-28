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

/*! \file PALETTECOLORToRGB.h
    \brief Declaration of the class PALETTECOLORToRGB.

*/

#if !defined(imebraPALETTECOLORToRGB_E27C63E7_A907_4899_9BD3_8026AD7D110C__INCLUDED_)
#define imebraPALETTECOLORToRGB_E27C63E7_A907_4899_9BD3_8026AD7D110C__INCLUDED_

#include "colorTransform.h"
#include "LUT.h"

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

namespace colorTransforms
{

///////////////////////////////////////////////////////////
/// \brief Transforms an image from the colorspace 
///         PALETTE COLOR into the color space RGB.
///
/// The input image has to have the colorspace
///  PALETTE COLOR, while the output image is created by 
///  the transform and will have the colorspace RGB.
///
/// The transform needs a dataSet or a palette in order
///  to perform the conversion.
/// Use declareDataSet() to declare the dataSet or setLut()
///  to declare the palette.
///
///////////////////////////////////////////////////////////
class PALETTECOLORToRGB: public colorTransform
{
public:
	virtual void declareDataSet(ptr<dataSet> pDataSet);

	/// \brief Specify the palette to use for the conversion.
	///
	/// If the application doesn't call this function before
	///  calling doTransform(), then the transform uses the
	///  palette declared in the dataSet.
	///
	/// @param red    the lut that stores the red components 
	///                of the mapped values
	/// @param green  the lut that stores the green components 
	///                of the mapped values
	/// @param blue   the lut that stores the blue components 
	///                of the mapped values
	///
	///////////////////////////////////////////////////////////
	virtual void setLut(ptr<lut> red, ptr<lut> green, ptr<lut> blue);

	virtual void doTransform();

	virtual std::wstring getInitialColorSpace();
	virtual std::wstring getFinalColorSpace();
	virtual ptr<colorTransform> createColorTransform();

protected:
	virtual void doColorTransform(imbxInt32* pSourceMem, imbxInt32* pDestMem, imbxUint32 pixelsNumber, imbxInt32 inputMinValue, imbxInt32 inputMaxValue, imbxInt32 outputMinValue, imbxInt32 outputMaxValue);

	// Color palette
	///////////////////////////////////////////////////////////
protected:
	ptr<lut> m_redPalette;
	ptr<lut> m_greenPalette;
	ptr<lut> m_bluePalette;

};

class PALETTECOLORToRGBException: public colorTransformException
{
public:
	PALETTECOLORToRGBException(const std::string& message): colorTransformException(message){}
};

class PALETTECOLORToRGBExceptionWhoToldYouSo: public PALETTECOLORToRGBException
{
public:
	PALETTECOLORToRGBExceptionWhoToldYouSo(const std::string& message): PALETTECOLORToRGBException(message){}
};


} // namespace colorTransforms

} // namespace transforms

} // namespace imebra

} // namespace puntoexe

#endif // !defined(imebraPALETTECOLORToRGB_E27C63E7_A907_4899_9BD3_8026AD7D110C__INCLUDED_)
