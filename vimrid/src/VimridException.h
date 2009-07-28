#ifndef VIMRIDEXCEPTION_H_
#define VIMRIDEXCEPTION_H_

#include <string>
#include <sstream>

namespace vimrid
{

class VimridException : std::exception
{
public:
	std::string Message;
	VimridException(std::string message) throw();
	virtual ~VimridException() throw();
	virtual const char* what() const throw();
	static std::stringstream _buffer;
};

}

// Allow streams to be passed in to the exception.
#define THROW_VIMRID_EX(stream) \
{ \
	vimrid::VimridException::_buffer.str(""); \
	vimrid::VimridException::_buffer << stream; \
	throw vimrid::VimridException(vimrid::VimridException::_buffer.str()); \
}

#endif // VIMRIDEXCEPTION_H_
