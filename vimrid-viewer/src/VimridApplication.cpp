#include "VimridApplication.h"
#include <unistd.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <iostream>
#include "VimridMenu.h"

using namespace std;
using namespace vimrid::input;
using namespace vimrid::mri;

#define UI_RENDER_INDEX 10000

namespace vimrid
{

VimridApplication *VimridApplication::_singleton = NULL;

VimridApplication::VimridApplication(
	VimridSettings &settings,
	InputManager *inputManager)
	:
	ErrorCode(0),
	Title("Untitled VimridApplication"),
	FrameStartTime(0.0),
	PrintFps(false),
	FramesPerSecond(0),
	mSettings(settings),
	mInputManager(inputManager),
	mMriVisualiser(NULL),
	mDragController(NULL),
	mWindowSize(800, 600),
	mUtility(TD_WAND_ID, MOUSE_BUTTON_LEFT, TRACKD_BUTTON_1),
	mUiContainer(UI_RENDER_INDEX),
	mEnableStereo(settings.EnableStereo)
{
	if (_singleton)
	{
		// Very important to maintain the singleton!
		THROW_VIMRID_EX("Only one instance of VimridApplication type can be created.");
	}

	/* As the application is a singleton, this is safe to do. It will be used
	 * by other classes (e.g. sprites) so that we don't have to pass application
	 * as a parameter to every single class. */
	_singleton = this;

	// Add to utility so that other classes may use it.
	mUtility.SetInputManager(inputManager);

	// Add user interface container to sprites so UI is rendered and updated.
	mSpriteManager.Add(mUiContainer);
}

VimridApplication::~VimridApplication()
{
	delete mMriVisualiser;
	delete mDragController;
	delete mInputManager;
}

/* This public static method allows any other object to
 * get the VimridApplication singleton instance without it
 * being passed on intentionally.
 */
VimridApplication *VimridApplication::GetInstance()
{
	if (!_singleton)
	{
		THROW_VIMRID_EX("VimridApplication instance has not yet been created.");
	}

	return _singleton;
}

void VimridApplication::Init()
{
	mDragController = new DragController(
		*mInputManager, TD_WAND_ID, mWindowSize);

	mMriVisualiser = new MriVisualiser(
		mSpriteManager,
		mTextureManager,
		*mDragController,
		mWindowSize);

	mUtility.Init();
}

void VimridApplication::Run()
{
	FrameStartTime = mUtility.GetTimeMilliseconds();
}

void VimridApplication::UpdateAndRender()
{
	mUtility.SetDeltaTime(mUtility.GetTimeMilliseconds() - FrameStartTime);
	FrameStartTime = mUtility.GetTimeMilliseconds();

	mInputManager->Update();
	mDragController->Update();
	mUtility.Update();

	Update();

	updateFrameRate();

	Render();
}

void VimridApplication::Reshape()
{
	mInputManager->SetWindowSize(mWindowSize);
	mUiContainer.SetSize(mWindowSize);
}

void VimridApplication::FinishRender()
{
}

void VimridApplication::Exit()
{
}

void VimridApplication::UpdateSprites()
{
	mSpriteManager.Update();
}

void VimridApplication::RenderSprites()
{
	mSpriteManager.Render();
}

void VimridApplication::updateFrameRate()
{
	static int t0 = -1;
	static int frames = 0;
	VUInt32 t = (VUInt32)(mUtility.GetTimeMilliseconds() / 1000);

	if (t0 < 0)
	{
		t0 = t;
	}

	frames++;

	if (t - t0 >= 1)
	{
		GLfloat seconds = t - t0;
		FramesPerSecond = frames / seconds;
		t0 = t;
		frames = 0;

		if (PrintFps)
		{
			WRITE_DEBUG_TEXT("FPS: " << FramesPerSecond);
		}
	}
}

MriVisualiser &VimridApplication::GetMriVisualiser()
{
	if (mMriVisualiser == NULL)
	{
		THROW_VIMRID_EX("The MRI Visualiser has not been initialised.");
	}

	return *mMriVisualiser;
}

const DragController &VimridApplication::GetDragController() const
{
	if (mDragController == NULL)
	{
		THROW_VIMRID_EX("The Drag Controller has not been initialised.");
	}

	return *mDragController;
}

void VimridApplication::SetWindowSize(const VUInt32 &width, const VUInt32 &height)
{
	mWindowSize.Width = width;
	mWindowSize.Height = height;
	mUtility.SetWindowSize(mWindowSize);
}

void VimridApplication::OnControlShown(vimrid::ui::Control &control)
{
	if (mUiContainer.HasVisibleChild() && mDragController->IsEnabled())
	{
		DEBUG("Dragging disabled.");
		mDragController->Disable();
	}
}

void VimridApplication::OnControlHidden(vimrid::ui::Control &control)
{
	if (!mUiContainer.HasVisibleChild() && !mDragController->IsEnabled())
	{
		DEBUG("Dragging enabled.");
		mDragController->Enable();
	}
}

void VimridApplication::OnControlSelectPress(vimrid::ui::Control &control)
{
}

void VimridApplication::OnControlSelectRelease(vimrid::ui::Control &control)
{
}

}
