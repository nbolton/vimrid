#include "SdlApplication.h"
#include <X11/XKBlib.h>
#include <iostream>

namespace vimrid
{
namespace sdl
{

SdlApplication::SdlApplication(VimridSettings &settings)
	: VimridApplication(settings, new SdlInputManager())
{
	exitEventLoop = false;
}

SdlApplication::~SdlApplication()
{
}

void SdlApplication::Run()
{
	this->VimridApplication::Run();

	display = XOpenDisplay(GetSettings().DisplayName.c_str());
	if (!display)
	{
		if (GetSettings().DisplayName.empty())
		{
			THROW_VIMRID_EX("Could not open default X display.");
		}
		else
		{
			THROW_VIMRID_EX(
					"Could not open X display("
					<< GetSettings().DisplayName << ").");
		}
	}

	makeWindow();
	XMapWindow(display, window);
	glXMakeCurrent(display, window, context);

	/*// Ensure that the input manager doesn't get flooded by autorepeat.
	Bool* setSupported;
	Bool setResult = XkbSetDetectableAutoRepeat(display, True, setSupported);

	Bool* getSupported;
	Bool getResult = XkbGetDetectableAutoRepeat(display, getSupported);*/

	Init();
	eventLoop();

	glXDestroyContext(display, context);
	XDestroyWindow(display, window);
	XCloseDisplay(display);
}

void SdlApplication::FinishRender()
{
	glXSwapBuffers(display, window);
}

void SdlApplication::eventLoop()
{
	while (!exitEventLoop)
	{
		processEvents();
		UpdateAndRender();
	}
}

SdlInputManager *SdlApplication::GetSdlInputManager()
{
	return (SdlInputManager*)GetInputManager();
}

void SdlApplication::processEvents()
{
	while (XPending(display) > 0)
	{
		XEvent event;
		XNextEvent(display, &event);

		switch (event.type)
		{
			case ConfigureNotify:
				{
					VUInt32 width = event.xconfigure.width;
					VUInt32 height = event.xconfigure.height;
					SetWindowSize(width, height);
					Reshape();
				}
				break;

			case KeyPress:
				{
					int code = XLookupKeysym(&event.xkey, 0);
					GetSdlInputManager()->UpdateKeyDown(code);
				}
				break;

			case KeyRelease:
				{
					int code = XLookupKeysym(&event.xkey, 0);
					GetSdlInputManager()->UpdateKeyUp(code);
				}
				break;
		}
	}
}

bool SdlApplication::IsKeyPressed(int xKey)
{
	return GetSdlInputManager()->IsKeyPressed(xKey);
}

void SdlApplication::Init()
{
	this->VimridApplication::Init();
}

void SdlApplication::Update()
{
}

void SdlApplication::Render()
{
}

void SdlApplication::Reshape()
{
	this->VimridApplication::Reshape();
}

void SdlApplication::Exit()
{
	this->VimridApplication::Exit();
	exitEventLoop = true;
}

/*
 * Create an RGB, double-buffered window.
 * Return the window and context handles.
 */
void SdlApplication::makeWindow()
{
	int normalAttribs[] =
	{
		GLX_RGBA,
		GLX_RED_SIZE, 1,
		GLX_GREEN_SIZE, 1,
		GLX_BLUE_SIZE, 1,
		GLX_DOUBLEBUFFER,
		GLX_DEPTH_SIZE, 1,
		None
	};

	int stereoAttribs[] =
	{
		GLX_RGBA,
		GLX_RED_SIZE, 1,
		GLX_GREEN_SIZE, 1,
		GLX_BLUE_SIZE, 1,
		GLX_DOUBLEBUFFER,
		GLX_DEPTH_SIZE, 1,
		GLX_STEREO,
		None
	};

	int screenNumber = DefaultScreen(display);
	Window root = RootWindow(display,screenNumber);

	if (GetSettings().FullScreen)
	{
		WindowPoint.X = 0;
		WindowPoint.Y = 0;

		VUInt32 width = DisplayWidth(display, screenNumber);
		VUInt32 height = DisplayHeight(display, screenNumber);
		SetWindowSize(width, height);
	}

	XVisualInfo *visualInfo;
	if (GetSettings().EnableStereo)
	{
		visualInfo = glXChooseVisual( display, screenNumber, stereoAttribs );
	}
	else
	{
		visualInfo = glXChooseVisual( display, screenNumber, normalAttribs );
	}

	if (!visualInfo)
	{
		if (GetSettings().EnableStereo)
		{
			THROW_VIMRID_EX("Could not get an RGB, Double-buffered, Stereo visual.");
		}
		else
		{
			THROW_VIMRID_EX("Could not get an RGB, Double-buffered visual.");
		}
	}

	XSetWindowAttributes windowAttribs;
	windowAttribs.background_pixel = 0;
	windowAttribs.border_pixel = 0;
	windowAttribs.colormap = XCreateColormap(display, root, visualInfo->visual, AllocNone);
	windowAttribs.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;
	windowAttribs.override_redirect = GetSettings().FullScreen;
	unsigned long mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect;

	window = XCreateWindow(
		display, root, 0, 0,
		(unsigned int)GetWindowSize().Width,
		(unsigned int)GetWindowSize().Height,
		0, visualInfo->depth, InputOutput,
		visualInfo->visual, mask, &windowAttribs);

	XSizeHints sizeHints;
	sizeHints.x = (int)WindowPoint.X;
	sizeHints.y = (int)WindowPoint.Y;
	sizeHints.width  = (int)GetWindowSize().Width;
	sizeHints.height = (int)GetWindowSize().Height;
	sizeHints.flags = USSize | USPosition;
	XSetNormalHints(display, window, &sizeHints);
	XSetStandardProperties(
		display, window, Title.c_str(), Title.c_str(),
		None, (char **)NULL, 0, &sizeHints);

	context = glXCreateContext(display, visualInfo, NULL, True);
	if (!context)
	{
		THROW_VIMRID_EX("Could not create GLX context.");
	}

	XFree(visualInfo);
}

}
}
