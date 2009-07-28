/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 05-May-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef TRACKDBUTTON_H_
#define TRACKDBUTTON_H_

#include <vimrid.h>
#include "TrackdButtonId.h"

namespace vimrid
{
namespace input
{
class TrackdButton
{
public:
	TrackdButton(TrackdButtonId buttonId);
	virtual ~TrackdButton();
	VBool IsPressed() const { return mPressed; }
	VBool WasToggled() const { return mToggled; }
	void Update();
	void SetPressed(VBool pressed) { mPressed = pressed; }
	TrackdButtonId GetButtonId() const { return mButtonId; }
private:
	VBool mPressed;
	VBool mPressedLast;
	VBool mToggled;
	TrackdButtonId mButtonId;
};
}
}

#endif // TRACKDBUTTON_H_
