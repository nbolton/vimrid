#include "VimridSettings.h"
#include <string>
#include <iostream>

using namespace std;
using namespace vimrid;

VimridSettings::VimridSettings() :
	DemoVersion("cube-1"),
	FullScreen(false),
	EnableStereo(true),
	RunGlxgears(false),
	RunSee3dDemo(false),
	RunDemo(false),
	ShowHelp(false),
	DebugMode(false),
	TrackdTest(false)
{
}

VimridSettings::~VimridSettings()
{
}

void VimridSettings::ProcessMainArgs(int argc, char* argv[])
{
	this->ArgumentCount = argc;
	this->ArgumentVariables = argv;

	for (int i = 1; i < argc; i++)
	{
		string argString = argv[i];

		if (argString.compare("--help") == 0)
		{
			ShowHelp = true;
		}
		else if (argString.compare("--fullscreen") == 0)
		{
			FullScreen = true;
        }
		else if (argString.compare("--display") == 0)
		{
			DisplayName = argv[++i];
		}
		else if (argString.compare("--nostereo") == 0)
		{
			EnableStereo = false;
		}
		else if (argString.compare("--glxgears") == 0)
		{
			RunGlxgears = true;
		}
		else if (argString.compare("--see3d-demo") == 0)
		{
			RunSee3dDemo = true;
		}
		else if (argString.compare("--demo") == 0)
		{
			RunDemo = true;
			DemoVersion = argv[++i];
		}
		else if (argString.compare("--input") == 0)
		{
			Input = argv[++i];
		}
		else if (argString.compare("--output") == 0)
		{
			Output = argv[++i];
		}
		else if (argString.compare("--debug") == 0)
		{
			DebugMode = true;
		}
		else if (argString.compare("--trackd-test") == 0)
		{
			TrackdTest = true;
		}
		else
		{
			cout << "Warning: Unknown argument: " << argv[i] << endl;
			ShowHelp = true;
		}
	}
}
