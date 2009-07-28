/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 02-May-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "VimridUtility.h"
#include "input/InputManager.h"
#include "VimridApplication.h"
#include <stdlib.h>
#include <math.h>

using namespace vimrid::glut;
using namespace vimrid::input;

namespace vimrid
{

VimridUtility::VimridUtility(
	VUInt32 trackdCursorSensorId,
	MouseButton mouseSelectButton,
	TrackdButtonId trackdSelectButton)
:
	mDeltaTime(0),
	mTrackdCursorSensorId(trackdCursorSensorId),
	mTrackdSpeed(150.0, 250.0, 5.0),
	mMouseSelectButton(mouseSelectButton),
	mTrackdSelectButton(trackdSelectButton),
	mInputManager(NULL),
	mTrackdEnabled(false),
	mTrackdSetupFinished(false)
{
}

VimridUtility::~VimridUtility()
{
	if (mTrackdSetupFinished)
	{
		delete mWand;
		delete mGlasses;
	}
}

VBool VimridUtility::IsFullScreenActive()
{
	return getApplication().IsFullScreenActive();
}

VimridApplication &VimridUtility::getApplication()
{
	return *VimridApplication::GetInstance();
}

VimridUtility &VimridUtility::Get()
{
	 return getApplication().mUtility;
}

void VimridUtility::Init()
{
	if (mInputManager == NULL)
	{
		THROW_VIMRID_EX("Cannot initialise utility when input manager has not been set.");
	}

	if (IsTrackdSupported())
	{
		mInputManager->InitTrackd(TD_TRACKER_KEY);
		mGlasses = &mInputManager->AddTrackdDevice(TD_GLASSES_ID);
		mWand = &mInputManager->AddTrackdDevice(TD_WAND_ID, TD_WAND_CONTROLLER_KEY);
		mTrackdEnabled = true;
		mTrackdSetupFinished = true;
	}

	// Start with cursor in correct place (no animation until later).
	ResetCursor();
}

void VimridUtility::ResetCursor()
{
	mCursorPositionSmooth = GetCursorPosition();
}

void VimridUtility::EnableTrackd()
{
	if (!IsTrackdSupported())
	{
		THROW_VIMRID_EX("Trackd is not supported and so cannot be enabled.");
	}
	mTrackdEnabled = true;
	ResetCursor();
}

void VimridUtility::DisableTrackd()
{
	mTrackdEnabled = false;
	ResetCursor();
}

const GlutInputManager &VimridUtility::GetGlutInputManager() const
{
	GlutInputManager *gim = (GlutInputManager*)mInputManager;
	if (gim == NULL)
	{
		THROW_VIMRID_EX(
			"Cannot get the GLUT input manager, because " <<
			"the application is not a GLUT application.");
	}
	return *gim;
}

VBool VimridUtility::IsWandButtonPressed(const TrackdButtonId &button) const
{
	return mWand->GetController().IsButtonPressed(button);
}

VBool VimridUtility::IsGlutCharKeyPressed(CharKey charKey) const
{
	return GetGlutInputManager().IsCharKeyPressed(charKey);
}

const TrackdDeviceInfo &VimridUtility::GetTrackdDevice(VUInt32 sensorId) const
{
	if (!mTrackdSetupFinished)
	{
		THROW_VIMRID_EX("Cannot access devices before trackd has been setup.");
	}
	return mInputManager->GetTrackdDevice(sensorId);
}

const VPoint2f &VimridUtility::GetMousePosition() const
{
	return mInputManager->GetMouseInfo().GetPosition();
}

const VPoint3f &VimridUtility::GetTrackdSensorPosition(VUInt32 sensorId) const
{
	return GetTrackdSensor(sensorId).GetPosition();
}

VBool VimridUtility::IsTrackdSupported() const
{
	return mInputManager->IsTrackdSupported();
}

VPoint3f VimridUtility::GetTrackdCursorPosition() const
{
	return GetTrackdSensorPosition(mTrackdCursorSensorId) * mTrackdSpeed;
}

VBool VimridUtility::IsMouseButtonPressed(MouseButton mouseButton) const
{
	return mInputManager->GetMouseInfo().IsButtonPressed(mouseButton);
}


VBool VimridUtility::WasMouseButtonToggled(MouseButton mouseButton) const
{
	return mInputManager->GetMouseInfo().WasButtonToggled(mouseButton);
}

VBool VimridUtility::IsSelectButtonPressed() const
{
	if (IsTrackdEnabled())
	{
		const TrackdControllerInfo &c = GetTrackdController(mTrackdCursorSensorId);
		return c.IsButtonPressed(mTrackdSelectButton);
	}
	else
	{
		return IsMouseButtonPressed(mMouseSelectButton);
	}
}

VBool VimridUtility::WasSelectButtonToggled() const
{
	if (IsTrackdEnabled())
	{
		const TrackdControllerInfo &c = GetTrackdController(mTrackdCursorSensorId);
		return c.WasButtonToggled(mTrackdSelectButton);
	}
	else
	{
		return WasMouseButtonToggled(mMouseSelectButton);
	}
}

const TrackdControllerInfo &VimridUtility::GetTrackdController(VUInt32 sensorId) const
{
	return GetTrackdDevice(sensorId).GetController();
}

const TrackdSensorInfo &VimridUtility::GetTrackdSensor(VUInt32 sensorId) const
{
	return GetTrackdDevice(sensorId).GetSensor();
}

VPoint3f VimridUtility::GetCursorPosition() const
{
	if (IsTrackdEnabled())
	{
		return GetTrackdSensorPosition(mTrackdCursorSensorId) * mTrackdSpeed;
	}
	else
	{
		return GetMousePosition();
	}
}

VPoint3f VimridUtility::GetCursorPositionSmooth() const
{
	return mCursorPositionSmooth;
}

VUInt64 VimridUtility::GetTimeMilliseconds() const
{
	struct timeval tv;
	gettimeofday(&tv, NULL);

	/* Convert seconds to milliseconds, and then convert microseconds
	 * to milliseconds, and add the two together (as the two together
	 * indicate the total accurate time). */
	return (VUInt64)((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void VimridUtility::Update()
{
	// Simulate trackd twitch.
	/*srand(GetTimeMilliseconds());
	VFloat twitch = (rand() % 10 + 1) * 0.1;
	VPoint3f cursor = GetCursorPosition() + twitch;*/

	VPoint3f cursor = GetCursorPosition();
	const VFloat speed = 0.01;
	VPoint3f distance = (cursor - mCursorPositionSmooth);
	mCursorPositionSmooth += (distance * speed * GetDeltaTime());
}

}
