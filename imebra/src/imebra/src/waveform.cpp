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

/*! \file waveform.cpp
    \brief Implementation of the class waveform.

*/

#include "../include/waveform.h"
#include "../include/dataHandlerNumeric.h"
#include "../include/dataSet.h"

namespace puntoexe
{

namespace imebra
{

///////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////
waveform::waveform(ptr<dataSet> pDataSet):
	m_pDataSet(pDataSet)
{
}


///////////////////////////////////////////////////////////
//
// Returns the number of allocated bits
//
///////////////////////////////////////////////////////////
imbxUint32 waveform::getBitsAllocated()
{
	PUNTOEXE_FUNCTION_START(L"waveform::getBitsAllocated");

	return m_pDataSet->getUnsignedLong(0x5400, 0, 0x1004, 0);

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
//
// Returns the number of bits stored
//
///////////////////////////////////////////////////////////
imbxUint32 waveform::getBitsStored()
{
	PUNTOEXE_FUNCTION_START(L"waveform::getBitsStored");

	return m_pDataSet->getUnsignedLong(0x003A, 0, 0x021A, 0);

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
//
// Returns the number of channels
//
///////////////////////////////////////////////////////////
imbxUint32 waveform::getChannels()
{
	PUNTOEXE_FUNCTION_START(L"waveform::getChannels");

	return m_pDataSet->getUnsignedLong(0x003A, 0, 0x0005, 0);

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
//
// Returns the interpretation string
//
///////////////////////////////////////////////////////////
std::string waveform::getInterpretation()
{
	PUNTOEXE_FUNCTION_START(L"waveform::getChannels");

	return m_pDataSet->getString(0x5400, 0, 0x1006, 0);

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
//
// Returns the number of samples
//
///////////////////////////////////////////////////////////
imbxUint32 waveform::getSamples()
{
	PUNTOEXE_FUNCTION_START(L"waveform::getSamples");

	return m_pDataSet->getUnsignedLong(0x003A, 0, 0x0010, 0);

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
//
// Returns a data handler for the waveform
//
///////////////////////////////////////////////////////////
ptr<handlers::dataHandler> waveform::getIntegerData(imbxUint32 channel, imbxInt32 paddingValue)
{
	PUNTOEXE_FUNCTION_START(L"waveform::getIntegerData");

	static imbxInt32 uLawDecompressTable[256] =
	{
		-32124,-31100,-30076,-29052,-28028,-27004,-25980,-24956,
		-23932,-22908,-21884,-20860,-19836,-18812,-17788,-16764,
		-15996,-15484,-14972,-14460,-13948,-13436,-12924,-12412,
		-11900,-11388,-10876,-10364, -9852, -9340, -8828, -8316,
		-7932, -7676, -7420, -7164, -6908, -6652, -6396, -6140,
		-5884, -5628, -5372, -5116, -4860, -4604, -4348, -4092,
		-3900, -3772, -3644, -3516, -3388, -3260, -3132, -3004,
		-2876, -2748, -2620, -2492, -2364, -2236, -2108, -1980,
		-1884, -1820, -1756, -1692, -1628, -1564, -1500, -1436,
		-1372, -1308, -1244, -1180, -1116, -1052,  -988,  -924,
		-876,  -844,  -812,  -780,  -748,  -716,  -684,  -652,
		-620,  -588,  -556,  -524,  -492,  -460,  -428,  -396,
		-372,  -356,  -340,  -324,  -308,  -292,  -276,  -260,
		-244,  -228,  -212,  -196,  -180,  -164,  -148,  -132,
		-120,  -112,  -104,   -96,   -88,   -80,   -72,   -64,
		-56,   -48,   -40,   -32,   -24,   -16,    -8,     0,
		32124, 31100, 30076, 29052, 28028, 27004, 25980, 24956,
		23932, 22908, 21884, 20860, 19836, 18812, 17788, 16764,
		15996, 15484, 14972, 14460, 13948, 13436, 12924, 12412,
		11900, 11388, 10876, 10364,  9852,  9340,  8828,  8316,
		7932,  7676,  7420,  7164,  6908,  6652,  6396,  6140,
		5884,  5628,  5372,  5116,  4860,  4604,  4348,  4092,
		3900,  3772,  3644,  3516,  3388,  3260,  3132,  3004,
		2876,  2748,  2620,  2492,  2364,  2236,  2108,  1980,
		1884,  1820,  1756,  1692,  1628,  1564,  1500,  1436,
		1372,  1308,  1244,  1180,  1116,  1052,   988,   924,
		876,   844,   812,   780,   748,   716,   684,   652,
		620,   588,   556,   524,   492,   460,   428,   396,
		372,   356,   340,   324,   308,   292,   276,   260,
		244,   228,   212,   196,   180,   164,   148,   132,
		120,   112,   104,    96,    88,    80,    72,    64,
		56,    48,    40,    32,    24,    16,     8,     0
	};

	static imbxInt32 aLawDecompressTable[256] =
	{
		-5504, -5248, -6016, -5760, -4480, -4224, -4992, -4736,
		-7552, -7296, -8064, -7808, -6528, -6272, -7040, -6784,
		-2752, -2624, -3008, -2880, -2240, -2112, -2496, -2368,
		-3776, -3648, -4032, -3904, -3264, -3136, -3520, -3392,
		-22016,-20992,-24064,-23040,-17920,-16896,-19968,-18944,
		-30208,-29184,-32256,-31232,-26112,-25088,-28160,-27136,
		-11008,-10496,-12032,-11520,-8960, -8448, -9984, -9472,
		-15104,-14592,-16128,-15616,-13056,-12544,-14080,-13568,
		-344,  -328,  -376,  -360,  -280,  -264,  -312,  -296,
		-472,  -456,  -504,  -488,  -408,  -392,  -440,  -424,
		-88,   -72,   -120,  -104,  -24,   -8,    -56,   -40,
		-216,  -200,  -248,  -232,  -152,  -136,  -184,  -168,
		-1376, -1312, -1504, -1440, -1120, -1056, -1248, -1184,
		-1888, -1824, -2016, -1952, -1632, -1568, -1760, -1696,
		-688,  -656,  -752,  -720,  -560,  -528,  -624,  -592,
		-944,  -912,  -1008, -976,  -816,  -784,  -880,  -848,
		5504,  5248,  6016,  5760,  4480,  4224,  4992,  4736,
		7552,  7296,  8064,  7808,  6528,  6272,  7040,  6784,
		2752,  2624,  3008,  2880,  2240,  2112,  2496,  2368,
		3776,  3648,  4032,  3904,  3264,  3136,  3520,  3392,
		22016, 20992, 24064, 23040, 17920, 16896, 19968, 18944,
		30208, 29184, 32256, 31232, 26112, 25088, 28160, 27136,
		11008, 10496, 12032, 11520, 8960,  8448,  9984,  9472,
		15104, 14592, 16128, 15616, 13056, 12544, 14080, 13568,
		344,   328,   376,   360,   280,   264,   312,   296,
		472,   456,   504,   488,   408,   392,   440,   424,
		88,    72,   120,   104,    24,     8,    56,    40,
		216,   200,   248,   232,   152,   136,   184,   168,
		1376,  1312,  1504,  1440,  1120,  1056,  1248,  1184,
		1888,  1824,  2016,  1952,  1632,  1568,  1760,  1696,
		688,   656,   752,   720,   560,   528,   624,   592,
		944,   912,  1008,   976,   816,   784,   880,   848
	}; 

	// Lock the dataset during the interpretation of the 
	//  dataset
	///////////////////////////////////////////////////////////
	lockObject lockDataSet(m_pDataSet);

	// Get the original data
	///////////////////////////////////////////////////////////
	ptr<handlers::dataHandler> waveformData(m_pDataSet->getDataHandler(0x5400, 0x0, 0x1010, 0, false));
	std::string sourceDataType(waveformData->getDataType());
	
	// Get the interpretation, number of channels, number of
	//  samples
	///////////////////////////////////////////////////////////
	std::string waveformInterpretation(getInterpretation());
	imbxUint32 numChannels(getChannels());
	imbxUint32 numSamples(getSamples());
	imbxUint32 originalPaddingValue(0);
	bool bPaddingValueExists(false);
	ptr<handlers::dataHandler> paddingTagHandler(m_pDataSet->getDataHandler(0x5400, 0, 0x100A, 0, false));
	if(paddingTagHandler != 0)
	{
		originalPaddingValue = paddingTagHandler->getUnsignedLong();
		bPaddingValueExists = true;
	}

	
	// Allocate a buffer for the destination data
	///////////////////////////////////////////////////////////
	ptr<buffer> waveformBuffer(new buffer(0, "SL"));
	ptr<handlers::dataHandler> destinationHandler(waveformBuffer->getDataHandler(true, numSamples));

	// Copy the data to the destination for unsigned values
	///////////////////////////////////////////////////////////
	waveformData->setPointer(channel);
	if(sourceDataType == "UB" || sourceDataType == "US")
	{
		for(imbxUint32 copySamples (numSamples); copySamples != 0; --copySamples)
		{
			imbxUint32 unsignedData(waveformData->getUnsignedLong());
			waveformData->skip(numChannels);
			if(bPaddingValueExists && unsignedData == originalPaddingValue)
			{
				destinationHandler->setSignedLongIncPointer(paddingValue);
				continue;
			}
			destinationHandler->setUnsignedLongIncPointer(unsignedData);
		}
		destinationHandler->setPointer(0);
		return destinationHandler;
	}

	// Copy the data to the destination for signed values
	///////////////////////////////////////////////////////////
	int highBit(getBitsAllocated() - 1);
	imbxUint32 testBit = ((imbxUint32)1) << highBit;
	imbxUint32 orBits = ((imbxUint32)((imbxInt32)-1)) << highBit;
	for(imbxUint32 copySamples (numSamples); copySamples != 0; --copySamples)
	{
		imbxUint32 unsignedData = waveformData->getUnsignedLong();
		waveformData->skip(numChannels);
		if(bPaddingValueExists && unsignedData == originalPaddingValue)
		{
			destinationHandler->setSignedLongIncPointer(paddingValue);;
			continue;
		}
		if((unsignedData & testBit) != 0)
		{
			unsignedData |= orBits;
		}
		destinationHandler->setSignedLongIncPointer((imbxInt32)unsignedData);
	}

	destinationHandler->setPointer(0);

	// Now decompress uLaw or aLaw
	if(waveformInterpretation == "AB") // 8bits aLaw
	{
		for(imbxUint32 aLawSamples(numSamples); aLawSamples != 0; --aLawSamples)
		{
			imbxUint32 compressed(destinationHandler->getUnsignedLong());
			if(bPaddingValueExists && compressed == originalPaddingValue)
			{
				destinationHandler->incPointer();
				continue;
			}
			imbxInt32 decompressed(aLawDecompressTable[compressed]);
			destinationHandler->setSignedLongIncPointer(decompressed);
		}
	}

	// Now decompress uLaw or aLaw
	if(waveformInterpretation == "MB") // 8bits aLaw
	{
		for(imbxUint32 uLawSamples(numSamples); uLawSamples != 0; --uLawSamples)
		{
			imbxUint32 compressed(destinationHandler->getUnsignedLong());
			if(bPaddingValueExists && compressed == originalPaddingValue)
			{
				destinationHandler->incPointer();
				continue;
			}
			imbxInt32 decompressed(aLawDecompressTable[compressed]);
			destinationHandler->setSignedLongIncPointer(decompressed);
		}
	}

	destinationHandler->setPointer(0);
	return destinationHandler;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
//
// Returns the sequence item
//
///////////////////////////////////////////////////////////
ptr<dataSet> waveform::GetWaveformItem()
{
	return m_pDataSet;
}


} // namespace imebra

} // namespace puntoexe

