/*
 * SobelOperator.cpp
 *
 *  Created on: 06-Apr-2009
 *      Author: nick
 */

#include "SobelOperator.h"

namespace vimrid
{
namespace imaging
{
namespace processing
{

SobelOperator::SobelOperator() : ImageFilter(3, 3, 1)
{
	/*(*this)[0][0][0] = 1;
	(*this)[0][1][0] = 2;
	(*this)[0][2][0] = 1;
	(*this)[1][0][0] = 0;
	(*this)[1][1][0] = 0;
	(*this)[1][2][0] = 0;
	(*this)[2][0][0] = -1;
	(*this)[2][1][0] = -2;
	(*this)[2][2][0] = -1;*/

	/*(*this)[0][0][0] = -1;
	(*this)[0][1][0] = -0;
	(*this)[0][2][0] = +1;
	(*this)[1][0][0] = -2;
	(*this)[1][1][0] = 0;
	(*this)[1][2][0] = +2;
	(*this)[2][0][0] = -1;
	(*this)[2][1][0] = 0;
	(*this)[2][2][0] = +1;*/

	VInt32 pixels[3][3] =
	{
		{ +1, +2, +1 },
		{ 0, 0, 0 },
		{ -1, -2, -1 }
	};
	LoadPixels(*pixels);
}

SobelOperator::~SobelOperator()
{

}

}
}
}
