/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 23-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "DragController.h"
#include "DragMode.h"
#include "TrackdDeviceInfo.h"

namespace vimrid
{
namespace input
{

DragController::DragController(
	InputManager &inputManager,
	VUInt32 trackdSensorId,
	const VSizeF &windowSize)
	:
	mInputManager(inputManager),
	mTrackdSensorId(trackdSensorId),
	mWindowSize(windowSize),
	mEnabled(true)
{
	DragMode *dragMode1 = new DragMode(
		*this,
		MOUSE_BUTTON_LEFT,
		TRACKD_BUTTON_1,
		VPoint3f(160.0, 160.0, 20.0));

	DragMode *dragMode2 = new DragMode(
		*this,
		MOUSE_BUTTON_RIGHT,
		TRACKD_BUTTON_3,
		VPoint3f(80.0, 80.0, 80.0));

	mModes.push_back(dragMode1);
	mModes.push_back(dragMode2);
}

DragController::~DragController()
{
	for (VUInt32 i = 0; i < mModes.size(); i++)
	{
		delete mModes[i];
	}
}

void DragController::Update()
{
	if (mEnabled)
	{
		for (VUInt32 i = 0; i < mModes.size(); i++)
		{
			mModes[i]->Update();
		}
	}
}

void DragController::Reset()
{
	for (VUInt32 i = 0; i < mModes.size(); i++)
	{
		mModes[i]->Reset();
	}
}

const TrackdDeviceInfo &DragController::GetTrackdDevice()
{
	return mInputManager.GetTrackdDevice(mTrackdSensorId);
}

void DragController::Enable()
{
	mEnabled = true;
}

void DragController::Disable()
{
	mEnabled = false;
}

}
}
