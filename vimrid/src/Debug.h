/* Provides a standard method of outputting debug messages to the console.
 *
 * Created: 09-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <string>
#include <iostream>
#include <sstream>
#include "types/types.h"

namespace vimrid
{
class Debug
{
public:
	static vimrid::VBool _enableDebugMode;
	static void _writeDebugText();
	static std::stringstream _buffer;
};
}

// Allow writing of debug text from streams.
#define WRITE_DEBUG_TEXT(stream) \
{ \
	vimrid::Debug::_buffer << stream; \
	vimrid::Debug::_writeDebugText(); \
}

// Short hand macro for when code completion doesn't work.
#define DEBUG(stream) WRITE_DEBUG_TEXT(stream)

#endif // DEBUG_H_
