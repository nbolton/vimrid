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

/*! \file exception.cpp
    \brief Implementation of the exception classes.

*/

#include "../include/exception.h"
#include "../include/charsetConversion.h"

namespace puntoexe
{

///////////////////////////////////////////////////////////
// Force the construction of the exceptions manager before
//  main() starts
///////////////////////////////////////////////////////////
static exceptionsManager::forceExceptionsConstruction forceConstruction;

	
///////////////////////////////////////////////////////////
// Construct without increasing my reference counter
///////////////////////////////////////////////////////////
exceptionsManager::exceptionsManager(): baseObject(false)
{}

///////////////////////////////////////////////////////////
// Return the message info for the specified thread
///////////////////////////////////////////////////////////
std::wstring exceptionsManager::getMessage()
{
	tExceptionInfoList infoList;
	exceptionsManager::getExceptionInfo(&infoList);

	std::wstring message;
	for(tExceptionInfoList::iterator scanInfo = infoList.begin(); 
		scanInfo != infoList.end(); 
		++scanInfo)
	{
		message += scanInfo->getMessage();
		message += L"\n\n";
	}

	return message;
}


///////////////////////////////////////////////////////////
// Return the info objects for the specified thread
///////////////////////////////////////////////////////////
void exceptionsManager::getExceptionInfo(tExceptionInfoList* pList)
{
	ptr<exceptionsManager> pManager(getExceptionsManager());
	lockObject lock(pManager.get());

	tInfoMap::iterator findInformation = pManager->m_information.find(puntoexe::thread::getThreadId());
	if(findInformation == pManager->m_information.end())
	{
		return;
	}
	for(tExceptionInfoList::iterator scanInformation = findInformation->second.begin(); 
		scanInformation != findInformation->second.end(); 
		++scanInformation)
	{
		pList->push_back(*scanInformation);
	}
	pManager->m_information.erase(findInformation);
}


///////////////////////////////////////////////////////////
// Add an info object to the current thread
///////////////////////////////////////////////////////////
void exceptionsManager::addExceptionInfo(const exceptionInfo& info)
{
	ptr<exceptionsManager> pManager(getExceptionsManager());
	lockObject lock(pManager.get());
	pManager->m_information[puntoexe::thread::getThreadId()].push_back(info);
}


///////////////////////////////////////////////////////////
// Clears the information list for the current thread
///////////////////////////////////////////////////////////
void exceptionsManager::clearExceptionInfo()
{
	ptr<exceptionsManager> pManager(getExceptionsManager());
	lockObject lock(pManager.get());
	tInfoMap::iterator findInformation = pManager->m_information.find(puntoexe::thread::getThreadId());
	if(findInformation == pManager->m_information.end())
	{
		return;
	}
	pManager->m_information.erase(findInformation);
}

///////////////////////////////////////////////////////////
// Return a pointer to the exceptions manager
///////////////////////////////////////////////////////////
exceptionsManager* exceptionsManager::getExceptionsManager()
{
	static ptr<exceptionsManager> m_manager(new exceptionsManager);
	static exceptionsManager* const m_pManager(m_manager.get());
	
	return m_pManager;
}


///////////////////////////////////////////////////////////
// Construct the exceptionInfo object
///////////////////////////////////////////////////////////
exceptionInfo::exceptionInfo(std::wstring functionName, std::string fileName, long lineNumber, std::string exceptionType, std::string exceptionMessage):
	m_functionName(functionName), 
	m_fileName(fileName),
	m_lineNumber(lineNumber),
	m_exceptionType(exceptionType),
	m_exceptionMessage(exceptionMessage)
{}

///////////////////////////////////////////////////////////
// Construct the exceptionInfo object
///////////////////////////////////////////////////////////
exceptionInfo::exceptionInfo(): m_lineNumber(0)
{}
	
///////////////////////////////////////////////////////////
// Copy constructor
///////////////////////////////////////////////////////////
exceptionInfo::exceptionInfo(const exceptionInfo& right):
			m_functionName(right.m_functionName), 
			m_fileName(right.m_fileName),
			m_lineNumber(right.m_lineNumber),
			m_exceptionType(right.m_exceptionType),
			m_exceptionMessage(right.m_exceptionMessage)
{}

///////////////////////////////////////////////////////////
// Copy operator
///////////////////////////////////////////////////////////
exceptionInfo& exceptionInfo::operator=(const exceptionInfo& right)
{
	m_functionName = right.m_functionName;
	m_fileName = right.m_fileName;
	m_lineNumber = right.m_lineNumber;
	m_exceptionType = right.m_exceptionType;
	m_exceptionMessage = right.m_exceptionMessage;
	return *this;
}

///////////////////////////////////////////////////////////
// Return the exceptionInfo content in a string
///////////////////////////////////////////////////////////
std::wstring exceptionInfo::getMessage()
{
	charsetConversion convertUnicode;
	convertUnicode.initialize("ASCII");
	std::wostringstream message;
	message << "[" << m_functionName << "]" << "\n";
	message << " file: " << convertUnicode.toUnicode(m_fileName) << "  line: " << m_lineNumber << "\n";
	message << " exception type: " << convertUnicode.toUnicode(m_exceptionType) << "\n";
	message << " exception message: " << convertUnicode.toUnicode(m_exceptionMessage) << "\n";
	return message.str();
}



} // namespace puntoexe
