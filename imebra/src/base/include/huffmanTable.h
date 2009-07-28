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

/*! \file huffmanTable.h
    \brief Declaration of the huffman codec

*/

#if !defined(imebraHuffmanTable_3146DA5A_5276_4804_B9AB_A3D54C6B123A__INCLUDED_)
#define imebraHuffmanTable_3146DA5A_5276_4804_B9AB_A3D54C6B123A__INCLUDED_

#include "baseObject.h"
#include <vector>
#include <map>

///////////////////////////////////////////////////////////
//
// Everything is in the namespace puntoexe
//
///////////////////////////////////////////////////////////
namespace puntoexe
{
class streamReader;
class streamWriter;

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief This class calculates the huffman table from
///         a set of values and their frequency, and
///         can read or write huffman codes from/to a
///         baseStream object.
///
///////////////////////////////////////////////////////////
class huffmanTable: public baseObject
{
public:
	///////////////////////////////////////////////////////////
	/// \name Initialization
	///
	///////////////////////////////////////////////////////////
	//@{

	/// \brief Construct the huffman table and specify the
	///         maximum numbed of bits used by the values to
	///         be encoded.
	///
	/// @param maxValueLength the maximum lenght, in bits, of
	///                        the value that must be encoded.
	///                       Please note that this is not
	///                        the length of the huffman values
	///
	///////////////////////////////////////////////////////////
	huffmanTable(imbxUint32 maxValueLength);

	/// \brief Reset the internal data of the huffmanTable
	///         class.
	///
	/// The function removes all the calculated tables.
	/// The I/O functions readHuffmanCode() and 
	///  writeHuffmanCode() will not work until the tables are
	///  calculated by calcHuffmanCodesLength and 
	///  calcHuffmanTables().
	///
	///////////////////////////////////////////////////////////
	void reset();

	//@}

	
	///////////////////////////////////////////////////////////
	/// \name Huffman table generation
	///
	/// First, call incValueFreq() the right number of times
	///  for each value that appears in the stream, then
	///  call calcHuffmanCodesLength() and then
	///  calcHuffmanTables().
	///
	///////////////////////////////////////////////////////////
	//@{

	/// \brief Increase the frequency of a value that will be
	///         huffman encoded.
	///
	/// This function must be called for each time a value
	///  appears in a stream. Values with an higher frequency
	///  will have a shorted huffman code.
	///
	/// After the function has been called the right amount of
	///  times for every value that must be encoded, call 
	///  calcHuffmanCodesLength() and then calcHuffmanTables().
	///
	/// @param value   the value for wich the frequency must
	///                 be increased.
	///                The number of bits that form the value
	///                 must be less or equal to the number
	///                 of bits specified in the constructor
	///                 huffmanTable()
	///
	///////////////////////////////////////////////////////////
	void incValueFreq(const imbxUint32 value);

	/// \brief Calculates the length of the huffman codes.
	///
	/// This function must be called after incValueFreq() has
	///  been called to set the frequency of the values to be
	///  encoded.
	///
	/// After this function calculates the codes length, call
	///  calcHuffmanTables() to calculate the huffman codes 
	///  used by readHuffmanCode() and writeHuffmanCode().
	///
	/// @param maxCodeLength the maximum length in bits of the
	///                       generated huffman codes.
	///
	///////////////////////////////////////////////////////////
	void calcHuffmanCodesLength(const imbxUint32 maxCodeLength);
	
	/// \brief Generates the huffman table used by 
	///         readHuffmanCode() and writeHuffmanCode().
	///
	/// This function need the codes length generated by
	///  calcHuffmanCodesLength(): it will not work if the
	///  code lengths are not available.
	///
	///////////////////////////////////////////////////////////
	void calcHuffmanTables();
	
	/// \brief Remove the code with the higher value and the
	///         longer code from the code lengths table.
	///
	/// This function is usefull when extra data has been
	///  inserted through incValueFreq() but must not generate
	///  an huffman code.
	///
	/// E.g.: the jpeg coded insert an extra value with low
	///       frequency to reserve the last generated huffman 
	///       code, so the reserver huffman code will never
	///       be generated
	///
	///////////////////////////////////////////////////////////
	void removeLastCode();

	//@}


	///////////////////////////////////////////////////////////
	/// \name Huffman I/O
	///
	///////////////////////////////////////////////////////////
	//@{
	
	/// \brief Read and decode an huffman code from the 
	///         specified stream.
	///
	/// The function throws a huffmanExceptionRead exception
	///  if the read code cannot be decoded.
	///
	/// @param pBuffer  a pointer to the location where the
	///                  decoded value will be stored
	/// @param pStream  a pointer to the stream reader used to
	///                  read the code
	///
	///////////////////////////////////////////////////////////
	bool readHuffmanCode(imbxUint32* pBuffer, streamReader* pStream);

	/// \brief Write an huffman code to the specified stream.
	///
	/// The function throws a huffmanExceptionWrite exception
	///  if the specified value cannot be encoded.
	///
	/// @param pBuffer  a pointer to the value to be encoded 
	///                  and written to the stream
	/// @param pStream  a pointer to the stream writer used to
	///                  write the code
	///
	///////////////////////////////////////////////////////////
	void writeHuffmanCode(imbxUint32* pBuffer, streamWriter* pStream);

	//@}

protected:
	class valueObject
	{
	public:
		valueObject():m_freq(0), m_codeLength(0), m_nextCode(-1){}
		valueObject(const valueObject& right):m_freq(right.m_freq), m_codeLength(right.m_codeLength), m_nextCode(right.m_nextCode){}
		imbxUint32 m_freq;
		imbxUint32 m_codeLength;
		imbxInt32 m_nextCode;
	};

	class freqValue
	{
	public:
		freqValue(imbxUint32 freq = 0, imbxUint32 value = 0):m_freq(freq), m_value(value){}
		freqValue(const freqValue& right):m_freq(right.m_freq), m_value(right.m_value){}

		imbxUint32 m_freq;
		imbxUint32 m_value;
	};
	struct freqValueCompare
	{
		const bool operator()(const freqValue& left, const freqValue& right) const
		{
			return left.m_freq < right.m_freq || (left.m_freq == right.m_freq && left.m_value > right.m_value);
		}
	};

	class lengthValue
	{
	public:
		lengthValue(imbxUint32 length = 0, imbxUint32 value = 0):m_length(length), m_value(value){}
		lengthValue(const lengthValue& right):m_length(right.m_length), m_value(right.m_value){}

		imbxUint32 m_length;
		imbxUint32 m_value;
	};
	struct lengthValueCompare
	{
		const bool operator()(const lengthValue& left, const lengthValue& right) const
		{
			return left.m_length < right.m_length || (left.m_length == right.m_length && left.m_value < right.m_value);
		}
	};

	imbxUint32 m_numValues;

	// Values' frequency
	std::vector<valueObject> m_valuesFreq;
	
public:
	// Used to calculate the huffman codes
	std::vector<imbxUint32> m_orderedValues;
	imbxUint32 m_valuesPerLength[128];
	imbxUint32 m_minValuePerLength[128];
	imbxUint32 m_maxValuePerLength[128];

	// Final huffman table
	std::vector<imbxUint32> m_valuesToHuffman;
	std::vector<imbxUint32> m_valuesToHuffmanLength;

};

class huffmanException: public std::runtime_error
{
public:
	huffmanException(std::string message): std::runtime_error(message){}
};

class huffmanExceptionRead : public huffmanException
{
public:
	huffmanExceptionRead(std::string message): huffmanException(message){}
};

class huffmanExceptionWrite : public huffmanException
{
public:
	huffmanExceptionWrite(std::string message): huffmanException(message){}
};


} // namespace puntoexe


#endif // !defined(imebraHuffmanTable_3146DA5A_5276_4804_B9AB_A3D54C6B123A__INCLUDED_)
