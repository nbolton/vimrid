/*
 * SobelOperator.h
 *
 *  Created on: 06-Apr-2009
 *      Author: nick
 */

#ifndef SOBELOPERATOR_H_
#define SOBELOPERATOR_H_

#include "ImageFilter.h"

namespace vimrid
{
namespace imaging
{
namespace processing
{

class SobelOperator: public ImageFilter
{
public:
	SobelOperator();
	virtual ~SobelOperator();
};

}
}
}

#endif // SOBELOPERATOR_H_
