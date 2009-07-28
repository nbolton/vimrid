/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 23-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef DRAGCONTROLLER_H_
#define DRAGCONTROLLER_H_

#include "InputManager.h"
#include <vimrid.h>

namespace vimrid
{
namespace input
{

class DragMode;

class DragController
{
public:
	DragController(
		vimrid::input::InputManager &inputManager,
		VUInt32 trackdSensorId,
		const VSizeF &windowSize);
	virtual ~DragController();
	void Update();
	const DragMode &GetMode(VUInt32 modeId) const { return *mModes[modeId]; }
	std::vector<DragMode*> GetModes() const { return mModes; }
	vimrid::input::InputManager &GetInputManager() { return mInputManager; }
	const vimrid::input::TrackdDeviceInfo &GetTrackdDevice();
	const VSizeF &GetWindowSize() const { return mWindowSize; }
	void Enable();
	void Disable();
	VBool IsEnabled() const { return mEnabled; }
	void Reset();
private:
	vimrid::input::InputManager &mInputManager;
	std::vector<DragMode*> mModes;
	VUInt32 mTrackdSensorId;
	const VSizeF &mWindowSize;
	VBool mEnabled;
};

}
}

#endif // DRAGCONTROLLER_H_
