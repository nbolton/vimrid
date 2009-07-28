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

/*! \file stream.cpp
    \brief Implementation of the stream class.

*/

#include "../include/exception.h"
#include "../include/stream.h"
#include "../include/charsetConversion.h"
#include <sstream>

namespace puntoexe
{

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
//
// Stream
//
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Destructor
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
stream::~stream()
{
	if(m_openFile != 0)
	{
		::fclose(m_openFile);
	}
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Open a file (ansi)
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void stream::openFile(std::string fileName, int mode)
{
	PUNTOEXE_FUNCTION_START(L"stream::openFile (ansi)");

	std::wstring wFileName;
	size_t fileNameSize(fileName.size());
	wFileName.resize(fileNameSize);
	for(size_t copyChars = 0; copyChars != fileNameSize; ++copyChars)
	{
		wFileName[copyChars] = (wchar_t)fileName[copyChars];
	}
	openFile(wFileName, mode);

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Open a file (unicode)
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void stream::openFile(std::wstring fileName, int mode)
{
	PUNTOEXE_FUNCTION_START(L"stream::openFile (unicode)");
	if(m_openFile)
	{
		if(::fclose(m_openFile)!=0)
		{
			PUNTOEXE_THROW(streamExceptionClose, "Error while closing the file");
		}
		m_openFile = 0;
	}

	std::wstring strMode;

	int tempMode = mode & (~std::ios::binary);

	if(tempMode == (int)(std::ios::in | std::ios::out))
	{
		strMode = L"r+";
	}

	if(tempMode == (int)(std::ios::in | std::ios::out | std::ios::app))
	{
		strMode = L"a+";
	}
	
	if(tempMode == (int)(std::ios::in | std::ios::out | std::ios::trunc))
	{
		strMode = L"w+";
	}
	
	if(tempMode == (int)(std::ios::out) || tempMode == (int)(std::ios::out | std::ios::trunc))
	{
		strMode = L"w";
	}
	
	if(tempMode == (int)(std::ios::out | std::ios::app))
	{
		strMode = L"a";
	}
	
	if(tempMode == (int)(std::ios::in))
	{
		strMode = L"r";
	}

	strMode += L"b";

#ifdef WIN32
	m_openFile = ::_wfopen(fileName.c_str(), strMode.c_str());
#else
	// Convert the filename to UTF8
	charsetConversion toUtf8;
	toUtf8.initialize("ISO-IR 192");
	std::string utf8FileName(toUtf8.fromUnicode(fileName));

	// Convert the filemode to UTF8
	std::string utf8Mode(toUtf8.fromUnicode(strMode));
	
	m_openFile = ::fopen(utf8FileName.c_str(), utf8Mode.c_str());
#endif
	if(m_openFile == 0)
	{
		PUNTOEXE_THROW(streamExceptionOpen, "stream::openFile failure");
	}

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Write raw data into the stream
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void stream::write(imbxUint32 startPosition, imbxUint8* pBuffer, imbxUint32 bufferLength)
{
	PUNTOEXE_FUNCTION_START(L"stream::write");

	lockObject lockThis(this);

	::fseek(m_openFile, startPosition, SEEK_SET);
	if(ferror(m_openFile) != 0)
	{
		PUNTOEXE_THROW(streamExceptionWrite, "stream::seek failure");
	}

	if(::fwrite(pBuffer, 1, bufferLength, m_openFile) != bufferLength)
	{
		PUNTOEXE_THROW(streamExceptionWrite, "stream::write failure");
	}

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Read raw data from the stream
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
imbxUint32 stream::read(imbxUint32 startPosition, imbxUint8* pBuffer, imbxUint32 bufferLength)
{
	PUNTOEXE_FUNCTION_START(L"stream::read");

	lockObject lockThis(this);

	::fseek(m_openFile, startPosition, SEEK_SET);
	if(ferror(m_openFile) != 0)
	{
		return 0;
	}

	imbxUint32 readBytes = (imbxUint32)::fread(pBuffer, 1, bufferLength, m_openFile);
	if(ferror(m_openFile) != 0)
	{
		PUNTOEXE_THROW(streamExceptionRead, "stream::read failure");
	}
	return readBytes;

	PUNTOEXE_FUNCTION_END();
}


} // namespace puntoexe
