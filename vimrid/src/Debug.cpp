/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 09-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "Debug.h"

namespace vimrid
{

vimrid::VBool Debug::_enableDebugMode = false;
std::stringstream Debug::_buffer;

void Debug::_writeDebugText()
{
	if (_enableDebugMode)
	{
		std::cout << "Debug: " << _buffer.str() << std::endl;
		_buffer.str("");
	}
}

}
