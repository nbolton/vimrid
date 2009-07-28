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

/*! \file viewHelper.cpp
    \brief Implementation of the class view.

*/

#include "../include/viewHelper.h"
#include "../include/image.h"
#include "../include/colorTransformsFactory.h"
#include "../include/transformHighBit.h"

namespace puntoexe
{

namespace imebra
{

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
//
// cursorLine
//
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//
// Default constructors
//
///////////////////////////////////////////////////////////
cursorLine::cursorLine():
	m_x0(0), m_y0(0), m_x1(0), m_y1(0),
		m_red(255), m_green(0), m_blue(0),
		m_style(line), m_width(0)
{
}

///////////////////////////////////////////////////////////
//
// Construct and assign
//
///////////////////////////////////////////////////////////
cursorLine::cursorLine(
		imbxInt32 x0,
		imbxInt32 y0,
		imbxInt32 x1,
		imbxInt32 y1,
		imbxUint8 red,
		imbxUint8 green,
		imbxUint8 blue,
		tStyle style,
		imbxUint32 width):
	m_x0(x0), m_y0(y0), m_x1(x1), m_y1(y1),
		m_red(red), m_green(green), m_blue(blue),
		m_style(style), m_width(width)
{
}

///////////////////////////////////////////////////////////
//
// Copy constructor
//
///////////////////////////////////////////////////////////
cursorLine::cursorLine(const cursorLine& right)
{
	copy(right);
}

///////////////////////////////////////////////////////////
//
// Operator =
//
///////////////////////////////////////////////////////////
cursorLine& cursorLine::operator=(const cursorLine& right)
{
	copy(right);
	return *this;
}

///////////////////////////////////////////////////////////
//
// Copy
//
///////////////////////////////////////////////////////////
void cursorLine::copy(const cursorLine& right)
{
	m_x0 = right.m_x0;
	m_y0 = right.m_y0;
	m_x1 = right.m_x1;
	m_y1 = right.m_y1;

	m_red = right.m_red;
	m_green = right.m_green;
	m_blue = right.m_blue;

	m_style = right.m_style;
	m_width = right.m_width;
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
//
// view
//
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////
view::view(imbxInt32 rowByteAlign, bool bBGR):
	m_bitmapAlign(rowByteAlign),
	m_bBGR(bBGR),
	m_leftPosition(0),
	m_topPosition(0),
	m_rightPosition(0),
	m_bottomPosition(0),
	m_backgroundRed(192),
	m_backgroundGreen(192),
	m_backgroundBlue(192),
	m_zoom(1.0),
	m_drawBitmap(new transforms::drawBitmap)
{
}


///////////////////////////////////////////////////////////
//
// Get the background color
//
///////////////////////////////////////////////////////////
void view::getBackgroundColor(imbxUint8* pRed, imbxUint8* pGreen, imbxUint8* pBlue)
{
	*pRed   = m_backgroundRed;
	*pGreen = m_backgroundGreen;
	*pBlue  = m_backgroundBlue;
}


///////////////////////////////////////////////////////////
//
// Set the background color
//
///////////////////////////////////////////////////////////
void view::setBackgroundColor(imbxUint8 red, imbxUint8 green, imbxUint8 blue)
{
	m_backgroundRed   = red;
	m_backgroundGreen = green;
	m_backgroundBlue  = blue;
	invalidate(-1, -1, -1, -1);
}


///////////////////////////////////////////////////////////
//
// Get the image drawing area
//
///////////////////////////////////////////////////////////
void view::getImageRect(imbxInt32* pLeft, imbxInt32* pTop, imbxInt32* pRight, imbxInt32* pBottom)
{
	*pLeft   = m_leftPosition;
	*pTop    = m_topPosition;
	*pRight  = m_rightPosition;
	*pBottom = m_bottomPosition;
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Zoom
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//
// Get the active zoom factor
//
///////////////////////////////////////////////////////////
double view::getZoomFactor()
{
	return m_zoom;
}


///////////////////////////////////////////////////////////
//
// Set the active zoom factor
//
///////////////////////////////////////////////////////////
void view::setZoomFactor(double zoomFactor, imbxInt32 centerPointX /* =-1 */, imbxInt32 centerPointY /* =-1 */)
{
	// Update the zoom factor
	///////////////////////////////////////////////////////////
	m_zoom = zoomFactor;

	// If the image hasn't been set, then don't do anything
	///////////////////////////////////////////////////////////
	if(m_originalImage == 0)
	{
		return;
	}

	// Not automatic zoom
	///////////////////////////////////////////////////////////
	if(m_zoom > 0)
	{
		updateImageRect(centerPointX, centerPointY);
		return;
	}

	// Automatic zoom
	///////////////////////////////////////////////////////////
	setScrollSize(1, 1, true);
	imbxUint32 imageSizeX, imageSizeY;
	m_originalImage->getSize(&imageSizeX, &imageSizeY);

	setZoomRect(0, 0, imageSizeX, imageSizeY);
}


///////////////////////////////////////////////////////////
//
// Zoom to the selected image's area. The rectangle measure
//  units are in image's pixels
//
///////////////////////////////////////////////////////////
void view::setZoomRect(imbxInt32 left, imbxInt32 top, imbxInt32 right, imbxInt32 bottom)
{
	if(m_originalImage == 0)
	{
		return;
	}

	if(left > right)
	{
		imbxInt32 temp = left;
		left = right;
		right = temp;
	}

	if(top > bottom)
	{
		imbxInt32 temp = top;
		top = bottom;
		bottom = temp;

	}
	imbxInt32 centerPointX = (right - left)/2 + left;
	imbxInt32 centerPointY = (bottom - top)/2 + top;

	imbxUint32 sizeX, sizeY;
	getWindowSize(&sizeX, &sizeY);

	double imageSizeMmX, imageSizeMmY;
	m_originalImage->getSizeMm(&imageSizeMmX, &imageSizeMmY);

	imbxUint32 imageSizeX, imageSizeY;
	m_originalImage->getSize(&imageSizeX, &imageSizeY);

	imbxUint32 horzDPI, vertDPI;
	getScreenDPI(&horzDPI, &vertDPI);

	if(imageSizeMmX == 0)
	{
		imageSizeMmX = (double)imageSizeX * 25.4 / (double)horzDPI;
	}
	if(imageSizeMmY == 0)
	{
		imageSizeMmY = (double)imageSizeY * 25.4 / (double)vertDPI;
	}

	double displaySizeMmX = imageSizeMmX * ((double)(right - left) / (double)imageSizeX);
	double displaySizeMmY = imageSizeMmY * ((double)(bottom - top) / (double)imageSizeY);
	double horzZoom = 0.95*(sizeX*25.4)/(displaySizeMmX*(double)horzDPI);
	double vertZoom = 0.95*(sizeY*25.4)/(displaySizeMmY*(double)vertDPI);

	m_zoom = (horzZoom < vertZoom) ? horzZoom : vertZoom;

	updateImageRect(centerPointX, centerPointY);

}


///////////////////////////////////////////////////////////
//
// Zoom in/out. The selected pixel will be centered into the
//  window
//
///////////////////////////////////////////////////////////
void view::zoomInOut(bool bZoomIn, imbxInt32 centerPointX /* =-1 */, imbxInt32 centerPointY /* =-1 */)
{
	if(bZoomIn)
	{
		m_zoom *= 2;
	}
	else
	{
		m_zoom /= 2;
	}
	updateImageRect(centerPointX, centerPointY);
}


///////////////////////////////////////////////////////////
//
// Returns true if the auto zoom is enabled
//
///////////////////////////////////////////////////////////
bool view::isAutoZoom()
{
	if(m_originalImage == 0 || m_leftPosition == m_rightPosition || m_topPosition == m_bottomPosition)
	{
		return false;
	}
	imbxUint32 sizeX, sizeY;
	getWindowSize(&sizeX, &sizeY);
	return (m_leftPosition >= 0 && m_rightPosition <= (imbxInt32)sizeX && m_topPosition >= 0 && m_bottomPosition <= (imbxInt32)sizeY);
}


///////////////////////////////////////////////////////////
//
// Get the current image's pixel displayed in the center of
//  the window
//
///////////////////////////////////////////////////////////
void view::getCenterPoint(imbxInt32* pCenterPointX, imbxInt32* pCenterPointY)
{
	// Reset the result values
	///////////////////////////////////////////////////////////
	*pCenterPointX = 0;
	*pCenterPointY = 0;

	if(m_originalImage == 0)
	{
		return;
	}

	imbxUint32 imageSizeX, imageSizeY;
	m_originalImage->getSize(&imageSizeX, &imageSizeY);

	// Nothing happens if the image is not valid
	///////////////////////////////////////////////////////////
	if(m_rightPosition - m_leftPosition == 0 || m_bottomPosition - m_topPosition == 0)
	{
		return;
	}

	// Get the window's width
	///////////////////////////////////////////////////////////
	imbxUint32 windowWidth  = 0;
	imbxUint32 windowHeight = 0;
	getWindowSize(&windowWidth, &windowHeight);

	// Get the scroll position
	///////////////////////////////////////////////////////////
	imbxInt32 scrollX = 0;
	imbxInt32 scrollY = 0;
	getScrollPosition(&scrollX, &scrollY);

	// Calculate the actual center point
	///////////////////////////////////////////////////////////
	*pCenterPointX = windowPosToImageX(scrollX + windowWidth / 2);
	*pCenterPointY = windowPosToImageY(scrollY + windowHeight / 2);
}


///////////////////////////////////////////////////////////
//
// Center the desidered image's pixel in the window
//
///////////////////////////////////////////////////////////
void view::setCenterPoint(imbxInt32 centerPointX, imbxInt32 centerPointY)
{
	if(m_originalImage == 0)
	{
		return;
	}

	// Calculate the size of the area occupied by the image on
	//  the screen
	///////////////////////////////////////////////////////////
	imbxInt32 imageAreaWidth = m_rightPosition - m_leftPosition;
	imbxInt32 imageAreaHeight = m_bottomPosition - m_topPosition;

	// Get the window's width
	///////////////////////////////////////////////////////////
	imbxUint32 windowWidth  = 0;
	imbxUint32 windowHeight = 0;
	getWindowSize(&windowWidth, &windowHeight);

	// Get the scroll position
	///////////////////////////////////////////////////////////
	imbxInt32 oldScrollPosX = 0;
	imbxInt32 oldScrollPosY = 0;
	getScrollPosition(&oldScrollPosX, &oldScrollPosY);

	imbxInt32 newScrollPosX = oldScrollPosX;
	imbxInt32 newScrollPosY = oldScrollPosY;

	// Get the image's size
	///////////////////////////////////////////////////////////
	imbxUint32 imageSizeX, imageSizeY;
	m_originalImage->getSize(&imageSizeX, &imageSizeY);

	// Calculate the new scroll position
	///////////////////////////////////////////////////////////
	if(centerPointX>=0)
	{
		newScrollPosX = imbxInt32((centerPointX * imageAreaWidth)/imageSizeX) + m_leftPosition - windowWidth/2;
	}

	if(centerPointY>=0)
	{
		newScrollPosY = imbxInt32((centerPointY * imageAreaHeight)/imageSizeY) + m_topPosition - windowHeight/2;
	}

	// Check if the scroll position is valid
	///////////////////////////////////////////////////////////
	if(newScrollPosX+(imbxInt32)windowWidth>imageAreaWidth)
	{
		newScrollPosX=imageAreaWidth-(imbxInt32)windowWidth;
	}
	if(newScrollPosY+(imbxInt32)windowHeight>imageAreaHeight)
	{
		newScrollPosY=imageAreaHeight-(imbxInt32)windowHeight;
	}
	if(newScrollPosX < 0)
	{
		newScrollPosX = 0;
	}
	if(newScrollPosY < 0)
	{
		newScrollPosY = 0;
	}
	if(oldScrollPosX!=newScrollPosX || oldScrollPosY!=newScrollPosY)
	{
		setScrollPosition(newScrollPosX, newScrollPosY);
	}

	// Set the new scroll position
	///////////////////////////////////////////////////////////
	if(newScrollPosX != oldScrollPosX || newScrollPosY != oldScrollPosY)
	{
		setScrollPosition(newScrollPosX, newScrollPosY);
	}
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Cursor
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//
// Begin the cursor definition
//
///////////////////////////////////////////////////////////
void view::startCursorDef()
{
	m_tempCursorLines.clear();
}

///////////////////////////////////////////////////////////
//
// End the cursor definition
//
///////////////////////////////////////////////////////////
void view::endCursorDef(imbxInt32 cursorHotSpotX, imbxInt32 cursorHotSpotY)
{
	// Remove the current cursor's lines
	///////////////////////////////////////////////////////////
	invalidateLines(&m_cursorLines);

	// Copy the temporary lines into cursor's lines
	///////////////////////////////////////////////////////////
	m_cursorLines.clear();
	for(tCursorLinesList::iterator copyLines = m_tempCursorLines.begin(); copyLines != m_tempCursorLines.end(); ++copyLines)
	{
		m_cursorLines.push_back(*copyLines);
	}
	m_tempCursorLines.clear();


	// Do we have to scroll the window because the hotspot is
	//  outside the visible area?
	///////////////////////////////////////////////////////////
	if(!isMouseCaptured() || m_originalImage == 0)
	{
		invalidateLines(&m_cursorLines);
		return;
	}

	// Convert the hotspot into window's coordinates
	///////////////////////////////////////////////////////////
	imbxInt32 scrollX, scrollY;
	getScrollPosition(&scrollX, &scrollY);

	imbxUint32 imageSizeX, imageSizeY;
	m_originalImage->getSize(&imageSizeX, &imageSizeY);

	imbxInt32 windowHotSpotX = cursorHotSpotX * (m_rightPosition - m_leftPosition) / imageSizeX +m_leftPosition - scrollX;
	imbxInt32 windowHotSpotY = cursorHotSpotY * (m_bottomPosition - m_topPosition) / imageSizeY +m_topPosition - scrollY;

	// Find the amount of scroll to execute
	///////////////////////////////////////////////////////////
	imbxUint32 windowSizeX, windowSizeY;
	getWindowSize(&windowSizeX, &windowSizeY);

	imbxInt32 limitX = windowSizeX / 10;
	imbxInt32 limitY = windowSizeY / 10;

	imbxInt32 executeScrollX = 0;
	imbxInt32 executeScrollY = 0;

	if(windowHotSpotX < limitX)
	{
		executeScrollX = windowHotSpotX - limitX;
	}
	if(windowHotSpotY < limitY)
	{
		executeScrollY = windowHotSpotY - limitY;
	}
	if(windowHotSpotX > ((imbxInt32)windowSizeX - limitX) )
	{
		executeScrollX = windowHotSpotX - (imbxInt32)windowSizeX + limitX;
	}
	if(windowHotSpotY > ((imbxInt32)windowSizeY - limitY) )
	{
		executeScrollY = windowHotSpotY - (imbxInt32)windowSizeY + limitY;
	}

	if(executeScrollX != 0 && executeScrollY != 0)
	{
		updateWindow();
		setScrollPosition(scrollX + executeScrollX, scrollY + executeScrollY);
	}

	invalidateLines(&m_cursorLines);
}


///////////////////////////////////////////////////////////
//
// End the cursor definition
//
///////////////////////////////////////////////////////////
void view::endCursorDef()
{
	// Remove the current cursor's lines
	///////////////////////////////////////////////////////////
	invalidateLines(&m_cursorLines);

	// Copy the temporary lines into cursor's lines
	///////////////////////////////////////////////////////////
	m_cursorLines.clear();
	for(tCursorLinesList::iterator copyLines = m_tempCursorLines.begin(); copyLines != m_tempCursorLines.end(); ++copyLines)
	{
		m_cursorLines.push_back(*copyLines);
	}
	m_tempCursorLines.clear();

	// Invalidate the new lines
	///////////////////////////////////////////////////////////
	invalidateLines(&m_cursorLines);
}


///////////////////////////////////////////////////////////
//
// Define a cursor's line
//
///////////////////////////////////////////////////////////
void view::defCursorLine(imbxInt32 startPointX, imbxInt32 startPointY, imbxInt32 endPointX, imbxInt32 endPointY, imbxUint8 colorRed, imbxUint8 colorGreen, imbxUint8 colorBlue, cursorLine::tStyle style /* = cursorLine::line */, int width /* =0 */)
{
	cursorLine newLine(startPointX, startPointY, endPointX, endPointY, colorRed, colorGreen, colorBlue, style, width);
	m_tempCursorLines.push_back(newLine);
}

///////////////////////////////////////////////////////////
//
// Invalidate the specified lines
//
///////////////////////////////////////////////////////////
void view::invalidateLines(tCursorLinesList* pLines)
{
	if(pLines->empty() || m_originalImage == 0)
	{
		return;
	}

	imbxInt32 scrollX, scrollY;
	getScrollPosition(&scrollX, &scrollY);

	imbxUint32 windowWidth, windowHeight;
	getWindowSize(&windowWidth, &windowHeight);

	imbxUint32 imageSizeX, imageSizeY;
	m_originalImage->getSize(&imageSizeX, &imageSizeY);

	for(tCursorLinesList::iterator scanLines = pLines->begin(); scanLines != pLines->end(); ++scanLines)
	{
		imbxInt32 lineLeft = scanLines->m_x0 * (m_rightPosition - m_leftPosition) / imageSizeX + m_leftPosition;
		imbxInt32 lineTop = scanLines->m_y0 * (m_bottomPosition - m_topPosition) / imageSizeY + m_topPosition;
		imbxInt32 lineRight = scanLines->m_x1 * (m_rightPosition - m_leftPosition) / imageSizeX + m_leftPosition;
		imbxInt32 lineBottom = scanLines->m_y1 * (m_bottomPosition - m_topPosition) / imageSizeY + m_topPosition;

		if(lineRight < lineLeft)
		{
			imbxInt32 temp = lineLeft;
			lineLeft = lineRight;
			lineRight = temp;
		}

		if(lineBottom < lineTop)
		{
			imbxInt32 temp = lineTop;
			lineTop = lineBottom;
			lineBottom = temp;
		}

		lineLeft   -= scanLines->m_width + 2;
		lineTop    -= scanLines->m_width + 2;
		lineRight  += scanLines->m_width + 2;
		lineBottom += scanLines->m_width + 2;

		lineLeft -= scrollX;
		lineTop -= scrollY;
		lineRight -= scrollX;
		lineBottom -= scrollY;

		if(lineLeft < 0)
		{
			lineLeft = 0;
		}

		if(lineTop < 0)
		{
			lineTop = 0;
		}

		if(lineRight > (imbxInt32)windowWidth)
		{
			lineRight = (imbxInt32)windowWidth;
		}

		if(lineBottom > (imbxInt32)windowHeight)
		{
			lineBottom = (imbxInt32)windowHeight;
		}

		if(lineLeft >= lineRight || lineTop >= lineBottom)
		{
			continue;
		}

		invalidate(lineLeft, lineTop, lineRight, lineBottom);
	}
}

///////////////////////////////////////////////////////////
//
// Translate the cursor's position into a
//  coordinate relative to the image's pixels
//
///////////////////////////////////////////////////////////
imbxInt32 view::windowPosToImageX(imbxInt32 windowPosX)
{
	if(m_originalImage == 0)
	{
		return 0;
	}

	imbxUint32 imageSizeX, imageSizeY;
	m_originalImage->getSize(&imageSizeX, &imageSizeY);

	imbxInt32 returnValue = ((windowPosX - m_leftPosition) * (imbxInt32)imageSizeX) / (m_rightPosition - m_leftPosition);
	if(returnValue < 0)
	{
		return 0;
	}
	if(returnValue > (imbxInt32)imageSizeX)
	{
		return (imbxInt32)imageSizeX;
	}
	return returnValue;
}

///////////////////////////////////////////////////////////
//
// Translate the cursor's position into a
//  coordinate relative to the image's pixels
//
///////////////////////////////////////////////////////////
imbxInt32 view::windowPosToImageY(imbxInt32 windowPosY)
{
	if(m_originalImage == 0)
	{
		return 0;
	}

	imbxUint32 imageSizeX, imageSizeY;
	m_originalImage->getSize(&imageSizeX, &imageSizeY);

	imbxInt32 returnValue = ((windowPosY - m_topPosition) * (imbxInt32)imageSizeY) / (m_bottomPosition - m_topPosition);
	if(returnValue < 0)
	{
		return 0;
	}
	if(returnValue > (imbxInt32)imageSizeY)
	{
		return (imbxInt32)imageSizeY;
	}
	return returnValue;
}


///////////////////////////////////////////////////////////
//
// Return a pixel's horizontal position in millimiters
//
///////////////////////////////////////////////////////////
double view::imagePosToMillimitersX(imbxInt32 imagePosX)
{
	if(m_originalImage == 0)
	{
		return 0;
	}

	imbxUint32 imageSizeX, imageSizeY;
	m_originalImage->getSize(&imageSizeX, &imageSizeY);

	if(imageSizeX == 0)
	{
		return 0;
	}

	double mmSizeX, mmSizeY;
	m_originalImage->getSizeMm(&mmSizeX, &mmSizeY);

	return mmSizeX * (double)imagePosX / (double)imageSizeX;
}


///////////////////////////////////////////////////////////
//
// Return a pixel's vertical position in millimiters
//
///////////////////////////////////////////////////////////
double view::imagePosToMillimitersY(imbxInt32 imagePosY)
{
	if(m_originalImage == 0)
	{
		return 0;
	}

	imbxUint32 imageSizeX, imageSizeY;
	m_originalImage->getSize(&imageSizeX, &imageSizeY);

	if(imageSizeY == 0)
	{
		return 0;
	}

	double mmSizeX, mmSizeY;
	m_originalImage->getSizeMm(&mmSizeX, &mmSizeY);

	return mmSizeY * (double)imagePosY / (double)imageSizeY;
}


///////////////////////////////////////////////////////////
//
// Return the pixel horizontal index at the specified
//  position in millimeters
//
///////////////////////////////////////////////////////////
imbxInt32 view::millimitersToImagePosX(double millimitersX)
{
	if(m_originalImage == 0)
	{
		return 0;
	}

	double mmSizeX, mmSizeY;
	m_originalImage->getSizeMm(&mmSizeX, &mmSizeY);
	if(mmSizeX == 0)
	{
		return 0;
	}

	imbxUint32 imageSizeX, imageSizeY;
	m_originalImage->getSize(&imageSizeX, &imageSizeY);

	return (imbxInt32)((double)imageSizeX * millimitersX / mmSizeX);
}


///////////////////////////////////////////////////////////
//
// Return the pixel vertical index at the specified
//  position in millimeters
//
///////////////////////////////////////////////////////////
imbxInt32 view::millimitersToImagePosY(double millimitersY)
{
	if(m_originalImage == 0)
	{
		return 0;
	}

	double mmSizeX, mmSizeY;
	m_originalImage->getSizeMm(&mmSizeX, &mmSizeY);
	if(mmSizeY == 0)
	{
		return 0;
	}

	imbxUint32 imageSizeX, imageSizeY;
	m_originalImage->getSize(&imageSizeX, &imageSizeY);

	return (imbxInt32)((double)imageSizeY * millimitersY / mmSizeY);
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Drawing functions
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//
// Draw the window's content
//
///////////////////////////////////////////////////////////
void view::draw(void* pDeviceContext, imbxInt32 left, imbxInt32 top, imbxInt32 right, imbxInt32 bottom)
{
	// Check if the image is visible in the specified area
	///////////////////////////////////////////////////////////
	if(m_originalImage == 0 ||
		left >= m_rightPosition ||
		top >= m_bottomPosition ||
		right <= m_leftPosition ||
		bottom <= m_topPosition)
	{
		drawRectangle(pDeviceContext, left, top, right, bottom, m_backgroundRed, m_backgroundGreen, m_backgroundBlue);
		return;
	}

	if(left==right || top==bottom)
	{
		return;
	}

	// Adjust the area to be drawn
	///////////////////////////////////////////////////////////
	imbxInt32 bitmapLeft   = left;
	imbxInt32 bitmapTop    = top;
	imbxInt32 bitmapRight  = right;
	imbxInt32 bitmapBottom = bottom;

	if(bitmapLeft < m_leftPosition) bitmapLeft = m_leftPosition;
	if(bitmapTop < m_topPosition) bitmapTop = m_topPosition;
	if(bitmapRight > m_rightPosition) bitmapRight = m_rightPosition;
	if(bitmapBottom > m_bottomPosition) bitmapBottom = m_bottomPosition;

	m_drawBitmap->declareBitmapType(m_rightPosition - m_leftPosition, m_bottomPosition - m_topPosition,
		bitmapLeft - m_leftPosition, bitmapTop - m_topPosition, bitmapRight - m_leftPosition, bitmapBottom - m_topPosition,
		m_bitmapAlign,
		m_bBGR);

	m_drawBitmap->doTransform();

	imbxInt32 bitmapWidth, bitmapHeight, bitmapRowLength;
	imbxUint8* pFinalBuffer = m_drawBitmap->getOutputBitmap(&bitmapWidth, &bitmapHeight, &bitmapRowLength);
	if(bitmapWidth != 0 && bitmapHeight != 0)
	{
		drawBitmap(pDeviceContext, bitmapLeft, bitmapTop, bitmapRight, bitmapBottom, bitmapWidth, bitmapHeight, bitmapRowLength, pFinalBuffer);
	}

	if(top < m_topPosition)
	{
		drawRectangle(pDeviceContext, left, top, right, m_topPosition, m_backgroundRed, m_backgroundGreen, m_backgroundBlue);
	}

	if(bottom > m_bottomPosition)
	{
		drawRectangle(pDeviceContext, left, m_bottomPosition, right, bottom, m_backgroundRed, m_backgroundGreen, m_backgroundBlue);
	}

	if(left < m_leftPosition)
	{
		drawRectangle(pDeviceContext, left, top, m_leftPosition, bottom, m_backgroundRed, m_backgroundGreen, m_backgroundBlue);
	}

	if(right > m_rightPosition)
	{
		drawRectangle(pDeviceContext, m_rightPosition, top, right, bottom, m_backgroundRed, m_backgroundGreen, m_backgroundBlue);
	}
}

///////////////////////////////////////////////////////////
//
// Draw the cursor
//
///////////////////////////////////////////////////////////
void view::drawCursor(void* pDeviceContext)
{
	if(m_originalImage == 0)
	{
		return;
	}

	imbxUint32 imageSizeX, imageSizeY;
	m_originalImage->getSize(&imageSizeX, &imageSizeY);

	for(tCursorLinesList::iterator scanLines = m_cursorLines.begin(); scanLines != m_cursorLines.end(); ++scanLines)
	{
		imbxInt32 x0 = scanLines->m_x0 * (m_rightPosition - m_leftPosition) / imageSizeX + m_leftPosition;
		imbxInt32 y0 = scanLines->m_y0 * (m_bottomPosition - m_topPosition) / imageSizeY + m_topPosition;
		imbxInt32 x1 = scanLines->m_x1 * (m_rightPosition - m_leftPosition) / imageSizeX + m_leftPosition;
		imbxInt32 y1 = scanLines->m_y1 * (m_bottomPosition - m_topPosition) / imageSizeY + m_topPosition;

		drawCursorLine(pDeviceContext,
			x0, y0, x1, y1,
			scanLines->m_red, scanLines->m_green, scanLines->m_blue,
			scanLines->m_style, scanLines->m_width);
	}
}


///////////////////////////////////////////////////////////
//
// Calculate the rectancle to use to draw the image
//
///////////////////////////////////////////////////////////
void view::updateImageRect(imbxInt32 centerPointX, imbxInt32 centerPointY)
{
	imbxInt32 tempCenterPointX = 0;
	imbxInt32 tempCenterPointY = 0;
	getCenterPoint(&tempCenterPointX, &tempCenterPointY);

	if(centerPointX < 0) centerPointX = tempCenterPointX;
	if(centerPointY < 0) centerPointY = tempCenterPointY;

	imbxInt32 leftPosition, topPosition, rightPosition, bottomPosition;
	leftPosition=
		topPosition=
		rightPosition=
		bottomPosition = 0;

	// Get the window's size
	///////////////////////////////////////////////////////////
	imbxUint32 windowSizeX = 0;
	imbxUint32 windowSizeY = 0;
	getWindowSize(&windowSizeX, &windowSizeY);

	// Get the scroll size
	///////////////////////////////////////////////////////////
	imbxUint32 scrollSizeX = 0;
	imbxUint32 scrollSizeY = 0;
	getScrollSize(&scrollSizeX, &scrollSizeY);

	// Get the scroll position
	///////////////////////////////////////////////////////////
	imbxInt32 scrollPosX = 0;
	imbxInt32 scrollPosY = 0;
	getScrollPosition(&scrollPosX, &scrollPosY);

	// For now, the new scroll size and position are the same
	//  as the old ones
	///////////////////////////////////////////////////////////
	imbxUint32 newScrollSizeX=scrollSizeX;
	imbxUint32 newScrollSizeY=scrollSizeY;

	if(m_originalImage != 0)
	{
		imbxUint32 imageSizeX(0), imageSizeY(0);
		m_originalImage->getSize(&imageSizeX, &imageSizeY);

		// Retrieve the screen's resolution
		///////////////////////////////////////////////////////////
		imbxUint32 screenHorzDPI, screenVertDPI;
		screenHorzDPI=screenVertDPI=75;
		getScreenDPI(&screenHorzDPI, &screenVertDPI);

		// Get the image's size (in mms and pixels)
		///////////////////////////////////////////////////////////
		double imageSizeMmX = 0;
		double imageSizeMmY = 0;
		m_originalImage->getSizeMm(&imageSizeMmX, &imageSizeMmY);

		if(imageSizeMmX == 0)
		{
			imageSizeMmX = (double)imageSizeX * 25.4 / (double)screenHorzDPI;
		}
		if(imageSizeMmY == 0)
		{
			imageSizeMmY = (double)imageSizeY * 25.4 / (double)screenVertDPI;
		}


		// Calculate the area occupied by the image, in screen's
		//  pixels
		///////////////////////////////////////////////////////////
		imbxUint32 displayAreaWidth=(imbxUint32)((double)imageSizeMmX*m_zoom*(double)screenHorzDPI/25.4+0.5);
		imbxUint32 displayAreaHeight=(imbxUint32)((double)imageSizeMmY*m_zoom*(double)screenVertDPI/25.4+0.5);

		if(displayAreaWidth>windowSizeX)
		{
			rightPosition = displayAreaWidth;
		}
		else
		{
			leftPosition = (windowSizeX-displayAreaWidth)>>1;
			rightPosition = leftPosition+displayAreaWidth;
		}

		if(displayAreaHeight>windowSizeY)
		{
			bottomPosition = displayAreaHeight;
		}
		else
		{
			topPosition = (windowSizeY-displayAreaHeight)>>1;
			bottomPosition = topPosition+displayAreaHeight;
		}
		newScrollSizeX = displayAreaWidth;
		newScrollSizeY = displayAreaHeight;
	}

	m_leftPosition = leftPosition;
	m_topPosition = topPosition;
	m_rightPosition = rightPosition;
	m_bottomPosition = bottomPosition;

	if(scrollSizeX!=newScrollSizeX || scrollSizeY!=newScrollSizeY)
	{
		setScrollSize(newScrollSizeX, newScrollSizeY, false);
	}

	setCenterPoint(centerPointX, centerPointY);

	invalidate(0, 0, windowSizeX, windowSizeY);
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set/get the active image
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//
// Retrieve the active image
//
///////////////////////////////////////////////////////////
ptr<image> view::getImage()
{
	return m_originalImage;
}

///////////////////////////////////////////////////////////
//
// Set the active image
//
///////////////////////////////////////////////////////////
void view::setImage(ptr<image> pImage, ptr<dataSet> pDataSet)
{
	if(pImage == 0)
	{
		m_originalImage = 0;
		setScrollSize(1, 1, true);
		m_drawBitmap->declareDataSet(0);
		return;
	}
	imbxUint32 oldSizeX = 0;
	imbxUint32 oldSizeY = 0;
	double oldSizeMmX = 0;
	double oldSizeMmY = 0;

	if(m_originalImage != 0)
	{
		m_originalImage->getSize(&oldSizeX, &oldSizeY);
		m_originalImage->getSizeMm(&oldSizeMmX, &oldSizeMmY);
	}

	m_originalImage = pImage;

	m_drawBitmap->declareDataSet(pDataSet);
	m_drawBitmap->declareInputImage(0, m_originalImage);

	imbxUint32 newSizeX, newSizeY;
	m_originalImage->getSize(&newSizeX, &newSizeY);
	double newSizeMmX, newSizeMmY;
	m_originalImage->getSizeMm(&newSizeMmX, &newSizeMmY);

	if(oldSizeX == newSizeX &&
		oldSizeY == newSizeY &&
		oldSizeMmX == newSizeMmX &&
		oldSizeMmY == newSizeMmY)
	{
		invalidate(m_leftPosition, m_topPosition, m_rightPosition, m_bottomPosition);
		return;
	}

	setZoomFactor(-1, -1, -1);
}


} // namespace imebra

} // namespace puntoexe
