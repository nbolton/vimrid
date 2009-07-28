/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 23-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "DragMode.h"
#include "DragController.h"
#include "InputManager.h"
#include "TrackdDeviceInfo.h"
#include "../VimridUtility.h"

namespace vimrid
{
namespace input
{

DragMode::DragMode(
	DragController &dragController,
	MouseButton mouseButton,
	TrackdButtonId trackdButton,
	const VPoint3f &trackdSpeed)
	:
	mDragController(dragController),
	mMouseButton(mouseButton),
	mTrackdButton(trackdButton),
	mDragActive(false),
	mTrackdSpeed(trackdSpeed)
{
}

DragMode::~DragMode()
{
}

InputManager &DragMode::getInputManager()
{
	return mDragController.GetInputManager();
}

const TrackdDeviceInfo &DragMode::getTrackdDevice()
{
	return mDragController.GetTrackdDevice();
}

const VSizeF &DragMode::getWindowSize()
{
	return mDragController.GetWindowSize();
}

VPoint3f DragMode::getDragDevicePoint()
{
	/*VPoint3f p;
	if (VimridUtility::Get().IsTrackdEnabled())
	{
		p = getTrackdDevice().GetSensor().GetPosition() * mTrackdSpeed;
	}
	else
	{
		p = getInputManager().GetMouseInfo().GetPosition();
	}
	return p;*/

	/* HACK: Ignores the trackd device specified by the drag controller. If
	 * anything other than the "cursor" device (as set in utility) is set
	 * then this would be used as opposed to the drag controller sensor. */
	return VimridUtility::Get().GetCursorPositionSmooth();
}

VBool DragMode::isDragButtonPressed()
{
	if (VimridUtility::Get().IsTrackdEnabled())
	{
		return getTrackdDevice().GetController().IsButtonPressed(mTrackdButton);
	}
	else
	{
		return getInputManager().GetMouseInfo().IsButtonPressed(mMouseButton);
	}
}

void DragMode::Update()
{
	if (!mDragActive && isDragButtonPressed())
	{
		// Drag isn't on but it needs to be, so turn it on.
		mDragActive = true;

		// Record where the mouse was when dragging started.
		mDragStart = getDragDevicePoint();

		WRITE_DEBUG_TEXT("Dragging active.");
	}

	if (mDragActive && !isDragButtonPressed())
	{
		// Drag is on, but button is off, so turn drag off.
		mDragActive = false;

		// Consolidate the amount of drag applied to the cursor.
		mDragCursor += mDragDelta;

		// Reset the delta as the drag state is already includes it.
		mDragDelta = 0;

		WRITE_DEBUG_TEXT("Dragging inactive.");
	}

	if (mDragActive)
	{
		/* Use the Y axis so we can scroll vertically, and calculate how much
		 * the mouse has moved, expressed as a factor of the window's height. */
		mDragDelta = (mDragStart - getDragDevicePoint());
	}
}

void DragMode::Reset()
{
	mDragCursor = 0;
}

}
}
