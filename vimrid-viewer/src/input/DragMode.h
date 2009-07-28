/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 23-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef DRAGMODE_H_
#define DRAGMODE_H_

#include "InputManager.h"
#include <vimrid.h>
#include "TrackdControllerInfo.h"

namespace vimrid
{
namespace input
{

class DragController;
class TrackdDeviceInfo;

class DragMode
{
public:
	DragMode(
		vimrid::input::DragController &dragController,
		MouseButton mouseButton,
		TrackdButtonId trackdButton,
		const VPoint3f &trackdSpeed);
	virtual ~DragMode();
	VPoint3f GetDragVector() const { return mDragCursor + mDragDelta; }
	VPoint3f GetDragStart() const { return mDragStart; }
	VBool IsDragActive() const { return mDragActive; }
	void Update();
	void Reset();
private:
	vimrid::input::DragController &mDragController;
	MouseButton mMouseButton;
	TrackdButtonId mTrackdButton;
	VPoint3f mDragCursor;
	VPoint3f mDragDelta;
	VPoint3f mDragStart;
	VBool mDragActive;
	VPoint3f mTrackdSpeed;
	VBool isDragButtonPressed();
	VPoint3f getDragDevicePoint();
	vimrid::input::InputManager &getInputManager();
	const vimrid::input::TrackdDeviceInfo &getTrackdDevice();
	const VSizeF &getWindowSize();
};

}
}

#endif // DRAGMODE_H_
