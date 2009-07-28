#ifndef LAUNCHER_H_
#define LAUNCHER_H_

#define ERROR_NONE 0
#define ERROR_EXCEPTION 1

#include "VimridSettings.h"

namespace vimrid
{

class VimridLauncher
{
public:
	VimridLauncher();
	virtual ~VimridLauncher();
	int Launch(int argc, char* argv[]);

private:
	VimridSettings mSettings;
	int runDemo(VimridSettings &settings);
};

}

#endif // LAUNCHER_H_
