/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 22-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef TRACKDCONTROLLERINFO_H_
#define TRACKDCONTROLLERINFO_H_

#include <vector>
#include <vimrid.h>
#include "TrackdButtonId.h"

namespace vimrid
{
namespace input
{
class TrackdButton;
class TrackdControllerInfo
{
public:
	TrackdControllerInfo();
	virtual ~TrackdControllerInfo();
	VBool IsButtonPressed(TrackdButtonId buttonId) const;
	VBool WasButtonToggled(TrackdButtonId buttonId) const;
	void Setup(void *controllerRef, VUInt32 buttonCount);
	void *GetControllerRef() { return mControllerRef; }
	VUInt32 GetButtonCount() { return mButtons.size(); }
	void UpdateButton(TrackdButtonId buttonId, VBool pressed);
	void Update();
private:
	void* mControllerRef;
	std::vector<TrackdButton*> mButtons;
	const TrackdButton &getButton(TrackdButtonId button) const;
};
}
}

#endif // TRACKDCONTROLLERINFO_H_
