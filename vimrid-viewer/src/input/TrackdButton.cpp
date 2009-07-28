/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 05-May-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "TrackdButton.h"

namespace vimrid
{
namespace input
{

TrackdButton::TrackdButton(TrackdButtonId buttonId)
:
	mButtonId(buttonId)
{
}

TrackdButton::~TrackdButton()
{
}

void TrackdButton::Update()
{
	// Store the toggle state, and remember state for next time.
	mToggled = mPressed != mPressedLast;
	mPressedLast = mPressed;

	if (mToggled)
		DEBUG("Trackd button (" << mButtonId << ") press toggle: " << mPressed);
}

}
}
