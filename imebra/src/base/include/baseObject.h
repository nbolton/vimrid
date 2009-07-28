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

/*! \file baseObject.h
    \brief Declaration of the base classes used by the puntoexe library.

*/

#if !defined(CImbxBaseObject_F1BAF067_21DE_466b_AEA1_6CC4F006FAFA__INCLUDED_)
#define CImbxBaseObject_F1BAF067_21DE_466b_AEA1_6CC4F006FAFA__INCLUDED_

#include "configuration.h"
#include "criticalSection.h"

#include <string>
#include <memory>

///////////////////////////////////////////////////////////
/// \namespace puntoexe
/// \brief The base services and classes are defined
///         in this namespace.
///
/// Other namespace in the library are usually children
///  of this namespace.
///
///////////////////////////////////////////////////////////
namespace puntoexe
{

class baseObject;

class basePtr
{
protected:
	// Default constructor
	//
	// Set the internal pointer to null.
	//
	///////////////////////////////////////////////////////////
	basePtr();

	// Construct the ptr object and keeps track of
	//         the object referenced by the pointer pObject.
	//
	// @param pObject a pointer to the allocated object.
	//        The allocated object will be automatically
	//         released by the class' destructor.\n
	//        The object must be allocated by the C++ new
	//         statement.\n
	//        The object's reference counter is increased by
	//         this function.
	//
	///////////////////////////////////////////////////////////
	basePtr(baseObject* pObject);

public:
	// The destructor.
	//
	// The reference to the tracked object is decreased
	//  automatically.
	//
	///////////////////////////////////////////////////////////
	virtual ~basePtr();

	// Release the tracked object and reset the
	//         internal pointer.
	//
	///////////////////////////////////////////////////////////
	void release();

protected:
	void addRef();

	// A pointer to the tracked object
	///////////////////////////////////////////////////////////
	baseObject* object;
};


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief This class must be used to allocate objects
///         derived from the class baseObject.
///
/// The object type specified in the template will be
///  deleted by this class when the object's reference
///  counter reaches zero.
///
/// The references counter is managed automatically
///  by this class.
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
template<class objectType>
class ptr: public basePtr
{
public:
	/// \brief Default constructor
	///
	/// Set the internal pointer to null.
	///
	///////////////////////////////////////////////////////////
	ptr(): basePtr(0){}

	/// \brief Construct the ptr object and keeps track of
	///         the object referenced by the pointer pObject.
	///
	/// @param pObject a pointer to the allocated object.
	///        The allocated object will be automatically
	///         released by the class' destructor.\n
	///        The object must be allocated by the C++ new
	///         statement.\n
	///        The object's reference counter is increased by
	///         this function.
	///
	///////////////////////////////////////////////////////////
	ptr(objectType* pObject): basePtr(pObject){}


	/// \brief Copy constructor
	///
	/// The object tracked by another ptr is copied into the
	///  current ptr and the reference counter is increased.
	///
	/// @param ptrSource  the source ptr object that must be
	///                    copied into the current \ref ptr.
	///
	///////////////////////////////////////////////////////////
	ptr (ptr<objectType> const &ptrSource):
		basePtr(ptrSource.object)
	{
	}

	/// \brief Copy the object tracked by another
	///         \ref ptr.
	///
	/// @param ptrSource the ptr object that is
	///         currently tracking the object to be tracked
	///         by this ptr object.
	///        The reference counter of the new object is
	///         increased.
	///        The object previously tracked by this ptr is
	///         released.
	/// @return a reference to the ptr object.
	///
	///////////////////////////////////////////////////////////
	ptr<objectType>& operator=(const ptr<objectType>& ptrSource)
	{
		if(ptrSource.object != object)
		{
			release();
			object = ptrSource.object;
			addRef();
		}
		return *this;
	}

	/// \brief Compare the pointer to the tracked object with
	///         the pointer specified in the parameter.
	///
	/// @param ptrCompare the pointer to be compared with the
	///                    pointer to the tracked object
	/// @return true if the ptrCompare pointer is equal to
	///                    the pointer of the tracked object
	///
	///////////////////////////////////////////////////////////
	inline bool operator == (const objectType* ptrCompare) const
	{
		return object == ptrCompare;
	}

	/// \brief Compare the pointer to the tracked object with
	///         the pointer tracked by another ptr.
	///
	/// @param ptrCompare the ptr tracking the object to be
	///                    compared
	/// @return true if the ptrCompare pointer is equal to
	///                    the pointer of the tracked object
	///
	///////////////////////////////////////////////////////////
	inline bool operator == (const ptr<objectType>& ptrCompare) const
	{
		return object == ptrCompare.object;
	}

	inline bool operator != (const objectType* ptrCompare) const
	{
		return object != ptrCompare;
	}

	inline bool operator != (const ptr<objectType>& ptrCompare) const
	{
		return object != ptrCompare.object;
	}

	/// \brief Return the pointer to the tracked object.
	///
	/// @return the pointer to the tracked object
	///
	///////////////////////////////////////////////////////////
	inline objectType* operator ->() const
	{
		return (objectType*)object;
	}

	/// \brief Return the pointer to the tracked object.
	///
	/// @return the pointer to the tracked object
	///
	///////////////////////////////////////////////////////////
	inline objectType* get() const
	{
		return (objectType*)object;
	}

	/// \brief Return the pointer to the tracked object,
	///         type casted in the specified class pointer.
	///
	/// The cast is static, so its validity is checked at
	///  compile time.
	///
	/// @return the pointer to the tracked object
	///
	///////////////////////////////////////////////////////////
	template <class destType>
		inline operator destType*() const
	{
		return static_cast<destType*>((objectType*)object);
	}

	/// \brief Return a new ptr object tracking the object
	///         tracked by this ptr. A type cast is performed
	///         if necessary.
	///
	/// The type cast is static, so its validity is checked at
	///  compile time.
	///
	/// @return the pointer to the tracked object
	///
	///////////////////////////////////////////////////////////
	template <class destType>
		inline operator ptr<destType>() const
	{
		return ptr<destType>(static_cast<destType*>((objectType*)object));
	}
};

class lockObject;
class lockMultipleObjects;
class exceptionsManager;

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief This is the base class of the library.
///        Almost all the classes in the library are
///         derived from this one.
///
/// The class supplies a basic mechanism for multithrading
///  locking and a reference counter, used by the helper
///  class \ref ptr.
///
/// At construction time the reference counter is set to
///  0 and must be incremented with addRef(); the helper
///  class ptr does this automatically.\n
///
/// Use release() to decrement the reference counter.\n
/// The object automatically destroy itself when the
///  reference counter reaches 0, but only if the function
///  preDelete() returns true.
///
/// An object of type baseObject should never be deleted
///  using the C++ statement "delete".
///
/// In order to manage the reference counter easily
///  your application should use the objects derived from
///  baseObject through the helper class \ref ptr.
///
/// The locking mechanism is exposed through the class
///  lockObject.
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class baseObject
{
	friend class lockObject;
	friend class lockMultipleObjects;
	friend class basePtr;
	friend class exceptionsManager;

public:
    /// \internal
	/// \brief Creates the baseObject object. The reference
	///         counter is set to 0.
	///
	/// The object should be unallocated by the release()
	///  function, not by deleting it using the C++ instruction
	///  delete.
	///
	/// In order to avoid mistakes and memory leaks, objects
	///  derived from baseObject should be assigned to a
	///  \ref ptr object.
	///
	///////////////////////////////////////////////////////////
	baseObject();

	/// \brief Creates the baseObject object and set an
	///         external object to be used for the lock.
	///        The reference counter is set to 0.
	///
	/// The object should be unallocated by the release()
	///  function, not by deleting it using the C++ instruction
	///  delete.
	///
	/// In order to avoid mistakes and memory leaks, objects
	///  derived from baseObject should be assigned to a
	///  \ref ptr object.
	///
	/// @param externalLock a pointer to the object to use
	///                      to lock this one
	///
	///////////////////////////////////////////////////////////
	baseObject(ptr<baseObject> externalLock);

	/// \brief Creates the baseObject object and set an
	///         external object to be used for the lock.
	///        The reference counter is set to 0.
	///
	/// The object should be unallocated by the release()
	///  function, not by deleting it using the C++ instruction
	///  delete.
	///
	/// In order to avoid mistakes and memory leaks, objects
	///  derived from baseObject should be assigned to a
	///  \ref ptr object.
	///
	/// @param bIncreaseExceptionsManager the exceptions
	///            manager references counter is increased if
	///            this value is true
	///
	///////////////////////////////////////////////////////////
private:
	baseObject(bool bIncreaseExceptionsManager);

public:
	/// \brief Retrieve the object used to lock this one.
	///
	/// Most of the times the baseObject uses itself during
	///  the lock, but sometimes it may uses external objects.
	/// See \ref imebra_locks for more information.
	///
	/// @return the object used to lock this one
	///
	///////////////////////////////////////////////////////////
	baseObject* getExternalLock();

	/// \brief Returns one if the reference count is set to 1.
	///
	/// @return true if the reference counter is set to 1
	///
	///////////////////////////////////////////////////////////
	bool isReferencedOnce();

protected:
	// The destructor is protected because the object must
	//  be deleted using release()
	///////////////////////////////////////////////////////////
	virtual ~baseObject();

protected:
	/// \internal
	/// \brief Pointer to an object used to lock this one, or
	///         0 if an external object is not used
	///
	///////////////////////////////////////////////////////////
	ptr<baseObject> m_externalLock;

private:
	/// \brief Increase the object's references counter.
	///
	/// This function is called by the smart pointer \ref ptr.
	///
	/// Each call to addRef() must be matched by a call to
	///  release().
	///
	///////////////////////////////////////////////////////////
	void addRef();

	/// \brief Decreases the object's references counter.
	///        When the references counter reaches zero, then
	///         the object is deleted.
	///
	/// This function is called by the smart pointer \ref ptr.
	///
	/// See also addRef().
	///
	///////////////////////////////////////////////////////////
	void release();

	/// \internal
	/// \brief This function is called by release() before the
	///         deletion of the object.
	///
	/// The base version of this function just returns true.
	/// When the function returns true then the function
	///  release() deletes the object normally when its
	///  references counter reaches 0, otherwise the object
	///  will NOT be deleted when the references counter
	///  reaches 0.
	///
	/// @return true if the object should be deleted when
	///          the references counter reaches 0, false if
	///          the object should NOT be deleted when the
	///          references counter reaches 0.
	///
	///////////////////////////////////////////////////////////
	virtual bool preDelete();

	/// \brief References counter.
	///
	/// When the object is created it is set to 0, then the
	///  smart pointer that controls the object increases it
	///  by calling addRef().
	///
	/// When the reference counter reaches zero then the object
	///  is deleted.
	///
	///////////////////////////////////////////////////////////
	volatile long m_lockCounter;

	// Lock/Unlock the objects.
	///////////////////////////////////////////////////////////
	void lock();
	void unlock();

	bool m_bValid;

	criticalSection m_criticalSection;

	ptr<exceptionsManager> m_pExceptionsManager;

};


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief Locks the access to an object of type
///         baseObject.
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class lockObject
{
public:
	/// \brief Lock the access to the pObject's attributes.
	///
	/// The lock is per thread: this means that if a lockObject
	///  successfully locks a baseObject class, then all the
	///  other threads in the application will enter in a
	///  wait mode when they try to lock the same object until
	///  the original lock is released.
	///
	/// @param pObject a pointer to the object to lock
	///
	///////////////////////////////////////////////////////////
	lockObject(baseObject* pObject);

	/// \brief Unlock the access to the locked object.
	///
	///////////////////////////////////////////////////////////
	virtual ~lockObject();

	/// \brief Release the lock on the locked object.
	///
	///////////////////////////////////////////////////////////
	void unlock();

protected:
	// Pointer to the locked object
	///////////////////////////////////////////////////////////
	baseObject* m_pObject;
};


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief Locks the access to several objects of type
///         baseObject.
///
/// The class lock ALL the objects listed in the
///  constructor's parameter.
/// If the objects cannot be locked at once, then all of
///  them are left unlocked until all of them can be locked
///  all togheter.
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class lockMultipleObjects
{
public:
	/// \brief Represents a std::list of pointers to the
	///         objects to be locked (see puntoexe::ptr).
	///
	///////////////////////////////////////////////////////////
	typedef std::list<ptr<baseObject> > tObjectsList;

	/// \brief Construct the locker and lock all the objects
	///         listed in the pObjectsList.
	///
	/// The function looks for the external lockers (if any).
	/// An object may be listed several times in the list.
	///
	/// @param pObjectsList  a list of pointers to the
	///                       objects to be locked
	///
	///////////////////////////////////////////////////////////
	lockMultipleObjects(tObjectsList* pObjectsList);

	/// \brief Destroy the locker and unlock all the locked
	///         objects
	///
	///////////////////////////////////////////////////////////
	virtual ~lockMultipleObjects();

	/// \brief Unlock all the locked objects
	///
	///////////////////////////////////////////////////////////
	void unlock();

protected:
	/// \internal
	/// \brief A list of locked critical sections
	///
	///////////////////////////////////////////////////////////
	std::auto_ptr<tCriticalSectionsList> m_pLockedCS;
};

} // namespace puntoexe

#endif // !defined(CImbxBaseObject_F1BAF067_21DE_466b_AEA1_6CC4F006FAFA__INCLUDED_)

