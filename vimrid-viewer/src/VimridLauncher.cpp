#include <string.h>
#include <iostream>
#include "VimridLauncher.h"
#include "VimridApplication.h"
#include "tools/glxgears.h"
#include "tools/spinning_cube.h"
#include "demos/VimridDemos.h"
#include "test/TrackdInputManagerTest.h"
#include "viewer/VimridViewer.h"

using namespace std;
using namespace vimrid::demos;
using namespace vimrid::demos::cubedemos;
using namespace vimrid::demos::filterdemos;
using namespace vimrid::demos::interface;
using namespace vimrid::demos::texturing;
using namespace vimrid::demos::mri;
using namespace vimrid::test;
using namespace vimrid::viewer;

namespace vimrid
{

VimridLauncher::VimridLauncher()
{
}

VimridLauncher::~VimridLauncher()
{
}

int VimridLauncher::Launch(int argc, char* argv[])
{
	// Gather initial mSettings from user.
	mSettings.ProcessMainArgs(argc, argv);

	// Enable the global debug message system.
	Debug::_enableDebugMode = mSettings.DebugMode;

	if (mSettings.ShowHelp)
	{
		cout
			<< "Available parameters:" << endl
			<< " --debug             Print debug messages to console." << endl
			<< " --fullscreen        Use full screen mode." << endl
			<< " --display [number]  Screen to use for full screen." << endl
			<< " --nostereo          Disable stereographic 3D mode." << endl
			<< " --glxgears          Run the OpenGL glxgears demo." << endl
			<< " --see3d-demo        See3D spinng cube trackd API demo." << endl
			<< " --cube-demo [name]  Run demo with name (cube-1 = default)." << endl
			<< " --trackd-test       Run the trackd test program." << endl;
		return 0;
	}
	else if (mSettings.RunGlxgears)
	{
		cout << endl << "Running glxgears demo (in stereo)" << endl
			<< "Copyright (C) 1999-2001  Brian Paul" << endl << endl;

		return main_glxgears(argc, argv);
	}
	else if (mSettings.RunSee3dDemo)
	{
		cout << endl << "Running See3D trackd API demo (spinning cube)" << endl
			<< "Copyright (C) 2005  Julien Guertault" << endl
			<< "(C) See3D Visualisation, Phil Summers, November 2007" << endl << endl;

		return main_spinning_cube(argc, argv);
	}
	else if (mSettings.RunDemo)
	{
		return runDemo(mSettings);
	}
	else if (mSettings.TrackdTest)
	{
		TrackdInputManagerTest t;
		t.Run();
		return 0;
	}
	else
	{
		// Tell the nice people who to thank.
		cout << endl << "ViMRID - Visualisation for Magnetic Resonance Imaging data" << endl
			<< "A medical visualisation tool for students of oncology and radiology" << endl
			<< "njb4@aber.ac.uk - Aberystwyth University" << endl
			<< "Copyright (C) 2009  Nick Bolton" << endl << endl;

		VimridViewer* viewer = new VimridViewer(mSettings);
		viewer->Run();
		VInt32 code = viewer->ErrorCode;
		delete viewer;
		return code;
	}
}

int VimridLauncher::runDemo(VimridSettings &mSettings)
{
	VimridApplication* demo = NULL;
	if (mSettings.DemoVersion == "cube-1")
	{
		demo = new CubeDemo1(mSettings);
	}
	else if (mSettings.DemoVersion == "cube-2")
	{
		demo = new CubeDemo2(mSettings);
	}
	else if (mSettings.DemoVersion == "cube-3")
	{
		demo = new CubeDemo3(mSettings);
	}
	else if (mSettings.DemoVersion == "cube-4")
	{
		demo = new CubeDemo4(mSettings);
	}
	else if (mSettings.DemoVersion == "cube-5")
	{
		demo = new CubeDemo5(mSettings);
	}
	else if (mSettings.DemoVersion == "cube-6")
	{
		demo = new CubeDemo6(mSettings);
	}
	else if (mSettings.DemoVersion == "cube-7")
	{
		demo = new CubeDemo7(mSettings);
	}
	else if (mSettings.DemoVersion == "cube-8")
	{
		demo = new CubeDemo8(mSettings);
	}
	else if (mSettings.DemoVersion == "cube-9")
	{
		demo = new CubeDemo9(mSettings);
	}
	else if (mSettings.DemoVersion == "filter-1")
	{
		demo = new FilterDemo1(mSettings);
	}
	else if (mSettings.DemoVersion == "filter-2")
	{
		demo = new FilterDemo2(mSettings);
	}
	else if (mSettings.DemoVersion == "filter-3")
	{
		demo = new FilterDemo3(mSettings);
	}
	else if (mSettings.DemoVersion == "filter-4")
	{
		demo = new FilterDemo4(mSettings);
	}
	else if (mSettings.DemoVersion == "interface-1")
	{
		demo = new InterfaceDemo1(mSettings);
	}
	else if (mSettings.DemoVersion == "interface-2")
	{
		demo = new InterfaceDemo2(mSettings);
	}
	else if (mSettings.DemoVersion == "texture-1")
	{
		demo = new TextureDemo1(mSettings);
	}
	else if (mSettings.DemoVersion == "texture-2")
	{
		demo = new TextureDemo2(mSettings);
	}
	else if (mSettings.DemoVersion == "texture-3")
	{
		demo = new TextureDemo3(mSettings);
	}
	else if (mSettings.DemoVersion == "texture-4")
	{
		demo = new TextureDemo4(mSettings);
	}
	else if (mSettings.DemoVersion == "mri-1")
	{
		demo = new MriDemo1(mSettings);
	}

	if (demo != NULL)
	{
		cout << endl << "Running " << demo->Title << endl
			<< "Copyright (C) 2009  Nick Bolton" << endl << endl;

		demo->Run();
		VUInt32 code = demo->ErrorCode;
		delete demo;
		return code;
	}
	else
	{
		cout << endl << "ViMRID demo "
			<< mSettings.DemoVersion
			<< " does not exist." << endl;
		return 1;
	}
}

}
