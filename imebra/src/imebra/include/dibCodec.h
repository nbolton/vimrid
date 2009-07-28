///////////////////////////////////////////////////////////
//  imebraDibCodec.h
//  Implementation of the Class imebraDibCodec
//  Created on:      09-nov-2003 11.50.07
//  Original author: Paolo Brandoli
///////////////////////////////////////////////////////////

#if !defined(imebraDibCodec_E4AEF19C_89BF_424f_91DB_C845AF788A29__INCLUDED_)
#define imebraDibCodec_E4AEF19C_89BF_424f_91DB_C845AF788A29__INCLUDED_

#include "codec.h"


///////////////////////////////////////////////////////////
//
// Everything is in the namespace puntoexe::imebra
//
///////////////////////////////////////////////////////////
namespace puntoexe
{

namespace imebra
{

namespace codecs
{

class dibCodec : public codec 
{

public:
	dibCodec();
	virtual ~dibCodec();


public:
	// virtual bool CanHandleTransferUID(char* pTransferSyntaxUID);

	virtual imbxUint32 compress();
	virtual imbxUint32 decompress();

};

} // namespace codecs

} // namespace imebra

} // namespace puntoexe

#endif // !defined(imebraDibCodec_E4AEF19C_89BF_424f_91DB_C845AF788A29__INCLUDED_)
