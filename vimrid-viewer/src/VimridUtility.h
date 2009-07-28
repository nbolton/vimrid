/* Intended to be used as a singleton in VimridApplication; provides
 * short-hand functions for features such as input, et al.
 *
 * Promotes low coupling of the application class, and the objects
 * that reside within. The singleton application class is designed
 * so that GLUT (which uses static call-backs) can update the
 * singleton application from out side of the instantiation. Instead
 * of also using the application singleton to access the delta time
 * from sprites and such, this will be accessed from a singleton
 * of this class instead.
 *
 * Low coupling prevents the sprite-related classes from needing to
 * reference the application class. This would be bad because the
 * inheritants of the VimridApplication class implement sprites.
 *
 * Having the short hand helper methods also stops the VimridApplication
 * class from becoming a golden hammer.
 *
 * Created: 02-May-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef VIMRIDUTILITY_H_
#define VIMRIDUTILITY_H_

#define TD_TRACKER_KEY 4126
#define TD_WAND_CONTROLLER_KEY 4127
#define TD_GLASSES_ID 0
#define TD_WAND_ID 1

#include <vimrid.h>
#include "input/TrackdControllerInfo.h"
#include "glut/GlutInputManager.h"
#include "input/MouseInfo.h"

namespace vimrid
{
class VimridApplication;
namespace input
{
class InputManager;
class TrackdDeviceInfo;
class TrackdSensorInfo;
}

class VimridUtility
{
public:
	VimridUtility(
		VUInt32 trackdCursorSensorId,
		vimrid::input::MouseButton mouseSelectButton,
		vimrid::input::TrackdButtonId trackdSelectButton);
	virtual ~VimridUtility();
	void Init();
	void Update();
	static VimridUtility& Get();
	void SetDeltaTime(const VFloat &deltaTime) { mDeltaTime = deltaTime; }
	const VFloat &GetDeltaTime() const { return mDeltaTime; }
	void SetInputManager(vimrid::input::InputManager *inputManager) { mInputManager = inputManager; }
	const vimrid::input::InputManager &GetInputManager() const { return *mInputManager; }
	const VSizeF &GetWindowSize() const { return mWindowSize; }
	void SetWindowSize(const VSizeF &windowSize) { mWindowSize = windowSize; }
	const vimrid::input::TrackdDeviceInfo &GetTrackdWand() const { return *mWand; }
	const vimrid::input::TrackdDeviceInfo &GetTrackdGlasses() const { return *mGlasses; }
	const vimrid::glut::GlutInputManager &GetGlutInputManager() const;
	VBool IsWandButtonPressed(const vimrid::input::TrackdButtonId &button) const;
	VBool IsGlutCharKeyPressed(vimrid::glut::CharKey charKey) const;
	VPoint3f GetTrackdCursorPosition() const;
	const vimrid::input::TrackdDeviceInfo &GetTrackdDevice(VUInt32 sensorId) const;
	const VPoint2f &GetMousePosition() const;
	const VPoint3f &GetTrackdSensorPosition(VUInt32 sensorId) const;
	VBool IsTrackdSupported() const;
	VBool IsMouseButtonPressed(vimrid::input::MouseButton mouseButton) const;
	VBool WasMouseButtonToggled(vimrid::input::MouseButton mouseButton) const;
	VBool IsSelectButtonPressed() const;
	VBool WasSelectButtonToggled() const;
	const vimrid::input::TrackdControllerInfo &GetTrackdController(VUInt32 sensorId) const;
	const vimrid::input::TrackdSensorInfo &GetTrackdSensor(VUInt32 sensorId) const;
	VBool IsTrackdSetupFinished() const { return mTrackdSetupFinished; }
	VBool IsTrackdEnabled() const { return mTrackdEnabled; }
	void EnableTrackd();
	void DisableTrackd();
	void ResetCursor();
	VPoint3f GetCursorPositionSmooth() const;
	VPoint3f GetCursorPosition() const;
	VUInt64 GetTimeMilliseconds() const;
	VBool IsFullScreenActive();
private:
	VFloat mDeltaTime;
	VSizeF mWindowSize;
	VUInt32 mTrackdCursorSensorId;
	VPoint3f mTrackdSpeed;
	vimrid::input::TrackdDeviceInfo *mGlasses;
	vimrid::input::TrackdDeviceInfo *mWand;
	vimrid::input::MouseButton mMouseSelectButton;
	vimrid::input::TrackdButtonId mTrackdSelectButton;
	vimrid::input::InputManager *mInputManager;
	VBool mTrackdEnabled;
	VBool mTrackdSetupFinished;
	VPoint3f mCursorPositionSmooth;
	static VimridApplication &getApplication();
};

}

#endif // VIMRIDUTILITY_H_
