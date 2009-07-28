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

/*! \file thread.h
    \brief Declaration of a class that represents the threads.

The class in this file hide the platform specific features and supply a common
 interface to the threads.

*/

#if !defined(CImbxThread_F1BAF067_21DE_466b_AEA1_6CC4F006FAFA__INCLUDED_)
#define CImbxThread_F1BAF067_21DE_466b_AEA1_6CC4F006FAFA__INCLUDED_

#include "baseObject.h"

#ifdef PUNTOEXE_WINDOWS
#include "windows.h"
#else
#include <pthread.h> // this must be the first included file
#endif

#include <exception>
#include <stdexcept>
#include <list>

#include <typeinfo>

namespace puntoexe
{

/// \brief This class represents a thread.
///
/// The thread can be started with thread::start().
/// Once the thread is started, the virtual functionù
///  threadFunction() is executed in a separate thread.
///
/// The code in threadFunction() can call shouldTerminate()
///  to know when the thread should be terminated 
///  gracefully as soon as possible.
///
/// The function terminate() can be called to inform the
///  thread that it should terminate.
/// terminate() is called automatically by the thread
///  destructor which also waits for the thread termination
///  before deallocating it and returning.
///
///////////////////////////////////////////////////////////
class thread: public baseObject
{
public:
	/// \brief Construct the thread.
	///
	/// Your application has to call start() in order to start
	///  the thread function.
	///
	///////////////////////////////////////////////////////////
	thread();

	/// \internal
	/// \brief Called when the thread's reference counter
	///         reaches zero.
	///
	/// Calls terminate() in order to signal to the thread that
	///  it has to terminate as soon as possible, then wait
	///  for the thread termination before returning.
	///
	/// @return always true
	///
	///////////////////////////////////////////////////////////
	virtual bool preDelete();

	/// \brief This function is executed in another thread when
	///         start() is called.
	///
	/// The code in this function can call shouldTerminate()
	///  to know when another thread has requested its
	///  termination.
	///
	/// This function has to be implemented in a derived class.
	///
	///////////////////////////////////////////////////////////
	virtual void threadFunction()=0;

	/// \brief Execute the function threadFunction() in a
	///         separate thread.
	///
	///////////////////////////////////////////////////////////
	void start();
	
	/// \brief Communicates to the thread that it should
	///         terminate as soon as possible.
	///
	/// A call to shouldTerminate() returns true after this
	///  function has been called.
	///
	///////////////////////////////////////////////////////////
	void terminate();
	
	/// \brief Returns true if the function threadFunction()
	///         should return as soon as possible.
	///
	/// @return true if the function threadFunction() should
	///          return as soon as possible
	///
	///////////////////////////////////////////////////////////
	bool shouldTerminate();

	/// \brief Return true if the function threadFunction() is
	///         running in its own thread.
	///
	/// @return true if the function threadFunction() is
	///          running in its own thread
	///
	///////////////////////////////////////////////////////////
	bool isRunning();


#ifdef PUNTOEXE_WINDOWS
	typedef DWORD tThreadId;
#else
	typedef void* tThreadId;
#endif

	/// \brief Return a number that identifies the active 
	///         thread.
	///
	/// Please note that this isn't the thread handle, but a
	///  number that is different for each running thread.
	///
	/// When a thread terminates the number could be used to
	///  identify another thread.
	///
	/// @return an id for the currently active thread
	///
	///////////////////////////////////////////////////////////
	static tThreadId getThreadId();

	/// \brief Switch to another thread.
	///
	/// The current thread is stopped and the following thrad
	///  scheduled to be executed on the processor is started.
	///
	///////////////////////////////////////////////////////////
	static void yield();

private:
#ifdef PUNTOEXE_WINDOWS
	static unsigned int __stdcall privateThreadFunction(void* pParameter);
#else
	static void* privateThreadFunction(void* pParameter);
#endif

	// This is the thread handle. Is used to join the thread
	///////////////////////////////////////////////////////////
#ifdef PUNTOEXE_WINDOWS
	HANDLE m_threadHandle;
#else
	pthread_t m_threadHandle;
#endif
	bool m_bThreadHandleValid;
	criticalSection m_lockThreadHandle;

	// true if the thread should terminate
	///////////////////////////////////////////////////////////
	bool m_bTerminate;
	criticalSection m_lockTerminateFlag;

	// true if the thread's function is running
	///////////////////////////////////////////////////////////
	bool m_bIsRunning;
	criticalSection m_lockRunningFlag;

#ifdef PUNTOEXE_POSIX
public:
	static pthread_key_t* getSharedKey();
	static void* getNextId();

	// Force the construction of the creation of a shared
	//  key for all the threads (pthread_key_create)
	///////////////////////////////////////////////////////////
	class forceKeyConstruction
	{
	public:
		forceKeyConstruction()
		{
			thread::getSharedKey();
			thread::getNextId();
		}
	};

private:
	class sharedKey
	{
	public:
		pthread_key_t m_key;
		sharedKey()
		{
			pthread_key_create(&m_key, 0);
		}
	};

#endif
};


/// \brief This is used as the base class for the 
///         exceptions thrown by the class thread.
///
///////////////////////////////////////////////////////////
class threadException: public std::runtime_error
{
public:
	threadException(std::string message): std::runtime_error(message){}
};

/// \brief Exception thrown by thread when an attempt to
///         run an already running thread is made.
///
///////////////////////////////////////////////////////////
class threadExceptionAlreadyRunning: public threadException
{
public:
	threadExceptionAlreadyRunning(std::string message): threadException(message){}
};


class threadExceptionFailedToLaunch: public threadException
{
public:
	threadExceptionFailedToLaunch(std::string message): threadException(message){}
};

} // namespace puntoexe

#endif // !defined(CImbxThread_F1BAF067_21DE_466b_AEA1_6CC4F006FAFA__INCLUDED_)
