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

/*! \file dicomDict.h
    \brief Declaration of the class dicomDict.

*/


#if !defined(imebraDicomDict_CC44A2C5_2B8C_42c1_9704_3F9C582643B9__INCLUDED_)
#define imebraDicomDict_CC44A2C5_2B8C_42c1_9704_3F9C582643B9__INCLUDED_

#include "../../base/include/baseObject.h"
#include <map>


///////////////////////////////////////////////////////////
//
// Everything is in the namespace puntoexe::imebra
//
///////////////////////////////////////////////////////////
namespace puntoexe
{

namespace imebra
{

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief The Dicom Dictionary.
///
/// This class can be used to retrieve the tags' default
///  data types and descriptions.
///
/// An instance of this class is automatically allocated
///  by the library: your application should use the
///  static function getDicomDictionary() in order to
///  get the only valid instance of this class.
/// 
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class dicomDictionary
{
	struct imageDataDictionaryElement
	{
		std::wstring m_tagName;
		std::string m_tagType;
	};

	struct validDataTypesStruct
	{
		bool  m_longLength;       // true if the tag has a 4 bytes length descriptor
		imbxUint32 m_wordLength;       // Word's length, used for byte reversing in hi/lo endian conversion
		imbxUint32 m_maxLength;        // The maximum length for the tag. An exception will be trown while reading a tag which exceedes this size 
	};

public:
	dicomDictionary();

	void registerTag(imbxUint32 tagId, const wchar_t* tagName, const char* tagType);
	void registerVR(std::string vr, bool bLongLength, imbxUint32 wordSize, imbxUint32 maxLength);

	/// \brief Retrieve a tag's description.
	///
	/// @param groupId   The group which the tag belongs to
	/// @param tagId     The tag's id
	/// @return          The tag's description
	///
	///////////////////////////////////////////////////////////
	std::wstring getTagName(imbxUint16 groupId, imbxUint16 tagId) const;

	/// \brief Retrieve a tag's default data type.
	///
	/// @param groupId   The group which the tag belongs to
	/// @param tagId     The tag's id
	/// @return          The tag's data type
	///
	///////////////////////////////////////////////////////////
	std::string getTagType(imbxUint16 groupId, imbxUint16 tagId) const;

	/// \brief Retrieve the only valid instance of this class.
	///
	/// @return a pointer to the dicom dictionary
	///
	///////////////////////////////////////////////////////////
	static dicomDictionary* getDicomDictionary();

	/// \brief Return true if the specified string represents
	///         a valid dicom data type.
	///
	/// @param dataType the string to be checked
	/// @return         true if the specified string is a valid
	///                  dicom data type
	///
	///////////////////////////////////////////////////////////
	bool isDataTypeValid(std::string dataType) const;

	/// \brief Return true if the tag's length in the dicom 
	///         stream must be written using a DWORD
	///
	/// @param dataType the data type for which the information
	///                  is required
	/// @return         true if the specified data type's 
	///                  length must be written using a DWORD
	///
	///////////////////////////////////////////////////////////
	bool getLongLength(std::string dataType) const ;
	
	/// \brief Return the size of the data type's elements
	///
	/// @param dataType the data type for which the information
	///                  is required
	/// @return the size of a single element
	///
	///////////////////////////////////////////////////////////
	imbxUint32 getWordSize(std::string dataType) const;
	
	/// \brief Return the maximum size of the tags with
	///         the specified data type.
	///
	/// @param dataType the data type for which the information
	///                  is required
	/// @return         the maximum tag's size in bytes 
	///
	///////////////////////////////////////////////////////////
	imbxUint32 getMaxSize(std::string dataType) const;

protected:
	typedef std::map<imbxUint32, imageDataDictionaryElement> tDicomDictionary;
	tDicomDictionary m_dicomDict;

	typedef std::map<std::string, validDataTypesStruct> tVRDictionary;
	tVRDictionary m_vrDict;

};

} // namespace imebra

} // namespace puntoexe

#endif // !defined(imebraDicomDict_CC44A2C5_2B8C_42c1_9704_3F9C582643B9__INCLUDED_)
