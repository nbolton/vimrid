/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 20-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef SCROLLCONTROLLER_H_
#define SCROLLCONTROLLER_H_

#include <vimrid.h>
#include "../input/DragController.h"

namespace vimrid
{

class VimridUtility;

namespace mri
{
enum ScrollMode
{
	SM_PUT_BACK,
	SM_LEAVE_OUT
};
class ScrollController
{
public:
	ScrollController(vimrid::input::DragController &dragController, VSizeF &windowSize);
	virtual ~ScrollController();
	void SetItemCount(VUInt32 itemCount) { mItemCount = itemCount; }
	VPoint3f GetItemPoint(VUInt32 itemIndex, const VimridUtility &deltaTime);
	void Reset();
	void ChangeScrollMode(ScrollMode scrollMode) { mScrollMode = scrollMode; }
private:
	vimrid::input::DragController &mDragController;
	VSizeF &mWindowSize;
	VUInt32 mItemCount;
	ScrollMode mScrollMode;
};

}
}

#endif // SCROLLCONTROLLER_H_
