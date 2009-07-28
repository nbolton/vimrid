#ifndef VIMRIDSETTINGS_H_
#define VIMRIDSETTINGS_H_

#include <string>

namespace vimrid
{
class VimridSettings
{
public:
	VimridSettings();
	virtual ~VimridSettings();
	void ProcessMainArgs(int argc, char* argv[]);
	std::string DisplayName;
	std::string DemoVersion;
	std::string Input;
	std::string Output;
	bool FullScreen;
	bool EnableStereo;
	bool RunGlxgears;
	bool RunSee3dDemo;
	bool RunDemo;
	bool ShowHelp;
	bool DebugMode;
	bool TrackdTest;
	int ArgumentCount;
	char** ArgumentVariables;
};
}

#endif // VIMRIDSETTINGS_H_
