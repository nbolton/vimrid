/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 20-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "ScrollController.h"
#include "../input/DragMode.h"

#define DRAG_MODE 1 // RMB

using namespace vimrid::input;

namespace vimrid
{
namespace mri
{

ScrollController::ScrollController(
	DragController &dragController,
	VSizeF &windowSize)
	:
	mDragController(dragController),
	mWindowSize(windowSize),
	mItemCount(0),
	mScrollMode(SM_PUT_BACK)
{
}

ScrollController::~ScrollController()
{
}

VPoint3f ScrollController::GetItemPoint(VUInt32 itemIndex, const VimridUtility &deltaTime)
{
	const VFloat dragSensitivity = 0.5;
	const VFloat sliceSlideTarget = 2.0;
	const VFloat slideReversePoint = 0.5;

	if (mItemCount == 0)
	{
		THROW_VIMRID_EX("Scroll controller cannot calculate item point when item count is zero.");
	}

	VPoint3f drag = mDragController.GetMode(DRAG_MODE).GetDragVector();
	VPoint3f p;

	/* Calculate which slice should be pulled out in relation to the
	 * current drag position, where 0 is the top of the screen and 1 is
	 * the bottom, multiplied by the number of slices. */
	VFloat dragSlice = ((drag.Y / mWindowSize.Height) * mItemCount) * dragSensitivity;

	if (mScrollMode == SM_PUT_BACK)
	{
		if ((dragSlice >= itemIndex) && (dragSlice <= (itemIndex + 1)))
		{
			VFloat sliceDelta = -(itemIndex - dragSlice);
			if (sliceDelta > slideReversePoint)
			{
				// Invert the delta.
				sliceDelta = 1 - sliceDelta;
			}

			// Multiply target by 2, as the slice will be put back.
			p.X = -(sliceDelta * sliceSlideTarget * 2);
		}
	}
	else if (mScrollMode == SM_LEAVE_OUT)
	{
		if ((dragSlice >= itemIndex) && (dragSlice <= (itemIndex + 1)))
		{
			VFloat sliceDelta = -(itemIndex - dragSlice);

			// Slide out exactly to the target (as much as the delta).
			p.X = -(sliceDelta * sliceSlideTarget);
		}
		else if (dragSlice > itemIndex)
		{
			// Set the position to the exact target (i.e. leave it out).
			p.X = -sliceSlideTarget;
		}
	}

	return p;
}

void ScrollController::Reset()
{
	mDragController.Reset();
	mItemCount = 0;
}

}
}
