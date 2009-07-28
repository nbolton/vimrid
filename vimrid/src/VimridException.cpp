#include "VimridException.h"

using namespace std;

namespace vimrid
{

std::stringstream VimridException::_buffer;

VimridException::VimridException(string message) throw()
{
	this->Message = message;
}

VimridException::~VimridException() throw()
{
}

const char* VimridException::what() const throw()
{
	return Message.c_str();
}

}
