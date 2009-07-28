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

/*! \file thread.cpp
    \brief Implementation of the thread-related functions.

*/

#include "../include/thread.h"
#include "../include/exception.h"
#include <map>
#include <memory>

#ifdef PUNTOEXE_WINDOWS
#include <process.h>
#endif

#ifdef PUNTOEXE_POSIX
#include <sched.h>
#include <errno.h>
#endif

namespace puntoexe
{

#ifdef PUNTOEXE_POSIX
static thread::forceKeyConstruction forceKey;
#endif

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
//
// thread
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
// Constructor
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
thread::thread():
    m_bThreadHandleValid(false),
        m_bTerminate(false),
        m_bIsRunning(false)
{
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Stop the thread before deallocating it
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
bool thread::preDelete()
{
	PUNTOEXE_FUNCTION_START(L"thread::preDelete");

	// If the thread's handle is not valid (not started yet),
	//  then return immediatly
	///////////////////////////////////////////////////////////
	{
		lockCriticalSection lockThreadHandle(&m_lockThreadHandle);
		if( !m_bThreadHandleValid )
		{
			return true;
		}
	}

	// Send a termination request to the thread's function
	///////////////////////////////////////////////////////////
	terminate();

#ifdef PUNTOEXE_WINDOWS

	// Wait for the thread termination, then close and
	//  invalidate the thread
	///////////////////////////////////////////////////////////
	::WaitForSingleObject(m_threadHandle, INFINITE);

	lockCriticalSection lockThreadHandle(&m_lockThreadHandle);
	::CloseHandle(m_threadHandle);
	m_bThreadHandleValid = false;

#else

	// Join the thread, then invalidate the handle
	///////////////////////////////////////////////////////////
	pthread_join(m_threadHandle, 0);

	lockCriticalSection lockThreadHandle(&m_lockThreadHandle);
	m_bThreadHandleValid = false;

#endif

	return true;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Function that is launched in a separate thread
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
#ifdef PUNTOEXE_WINDOWS

unsigned int __stdcall thread::privateThreadFunction(void* pParameter)

#else

void* thread::privateThreadFunction(void* pParameter)

#endif
{
	try
	{
		PUNTOEXE_FUNCTION_START(L"thread::privateThreadFunction");

		// Get the thread object that launched the function
		///////////////////////////////////////////////////////////
		thread* pThread = (thread*)pParameter;

		// Enable the "isRunning" flag
		///////////////////////////////////////////////////////////
		{
			lockCriticalSection lockRunningFlag(&(pThread->m_lockRunningFlag));
			pThread->m_bIsRunning = true;
		}

		// Call the virtual thread function
		///////////////////////////////////////////////////////////
		pThread->threadFunction();

		// Disable the "isRunning" flag
		///////////////////////////////////////////////////////////
		{
			lockCriticalSection lockRunningFlag(&(pThread->m_lockRunningFlag));
			pThread->m_bIsRunning = false;
		}

		PUNTOEXE_FUNCTION_END();
	}
	catch(...)
	{
		exceptionsManager::getMessage();
	}

	return 0;

}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Start the thread's function
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void thread::start()
{
	PUNTOEXE_FUNCTION_START(L"thread::start");

	lockCriticalSection lockThreadHandle(&m_lockThreadHandle);

	// Throw an exception if the thread is already running
	///////////////////////////////////////////////////////////
	if(m_bThreadHandleValid)
	{
		PUNTOEXE_THROW(threadExceptionAlreadyRunning, "Thread already running");
	}

#ifdef PUNTOEXE_WINDOWS
	// Start the thread
	///////////////////////////////////////////////////////////
	unsigned dummy;
	m_threadHandle = (HANDLE)::_beginthreadex(0, 0, thread::privateThreadFunction, this, 0, &dummy);
	if(m_threadHandle == 0)
	{
		PUNTOEXE_THROW(threadExceptionFailedToLaunch, "Failed to launch the thread");
	}
#else
	pthread_attr_t threadAttributes;
	pthread_attr_init(&threadAttributes);
	pthread_attr_setdetachstate(&threadAttributes, PTHREAD_CREATE_JOINABLE);
	int errorCode = pthread_create(&m_threadHandle, &threadAttributes, thread::privateThreadFunction, this);
	pthread_attr_destroy(&threadAttributes);

	if(errorCode != 0)
	{
		PUNTOEXE_THROW(threadExceptionFailedToLaunch, "Failed to launch the thread");
	}
#endif

	m_bThreadHandleValid = true;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Send a termination request to the running thread
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void thread::terminate()
{
	lockCriticalSection lockTerminateFlag(&m_lockTerminateFlag);
	m_bTerminate = true;
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Called by the thread's function to know if it has to
//  terminate
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
bool thread::shouldTerminate()
{
	lockCriticalSection lockTerminateFlag(&m_lockTerminateFlag);
	return m_bTerminate;
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Return true if the thread's function is running
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
bool thread::isRunning()
{
	lockCriticalSection lockRunningFlag(&m_lockRunningFlag);
	return m_bIsRunning;
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Return an indentifier for the current thread
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
thread::tThreadId thread::getThreadId()
{
#ifdef PUNTOEXE_WINDOWS
	return (tThreadId)::GetCurrentThreadId();
#else
	pthread_key_t* pKey = getSharedKey();
	void* id = pthread_getspecific(*pKey);
	if(id == 0)
	{
		id = getNextId();
		pthread_setspecific(*pKey, id);
	}
	return id;
#endif
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Switch to another thread
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void thread::yield()
{
#ifdef PUNTOEXE_WINDOWS // WINDOWS

#if(_WIN32_WINNT>=0x0400)
			SwitchToThread();
#else
			Sleep(0);
#endif

#else // POSIX
			sched_yield();
#endif
}


#ifdef PUNTOEXE_POSIX

pthread_key_t* thread::getSharedKey()
{
	static sharedKey m_key;
	return &(m_key.m_key);
}

void* thread::getNextId()
{
	static char* m_nextId(0);
	static criticalSection m_criticalSection;

	lockCriticalSection lockHere(&m_criticalSection);
	if(++m_nextId == 0)
	{
		m_nextId = (char*)100; // Overflow. Assume that the first created threads live longer
	}
	return (void*)m_nextId;
}

#endif

} // namespace puntoexe
