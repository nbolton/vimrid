/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 22-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "TrackdControllerInfo.h"
#include "TrackdButton.h"

using namespace std;

namespace vimrid
{
namespace input
{

TrackdControllerInfo::TrackdControllerInfo() :
	mControllerRef(NULL)
{
}

TrackdControllerInfo::~TrackdControllerInfo()
{
	for (VUInt32 i = 0; i < mButtons.size(); i++)
	{
		delete mButtons[i];
	}
}

const TrackdButton &TrackdControllerInfo::getButton(TrackdButtonId buttonId) const
{
	TrackdButton *found = NULL;
	for (VUInt32 i = 0; i < mButtons.size(); i++)
	{
		if (mButtons[i]->GetButtonId() == buttonId)
		{
			found = mButtons[i];
			break;
		}
	}
	if (found == NULL)
	{
		THROW_VIMRID_EX("Cannot find trackd controller button '" << buttonId << "'.");
	}
	return *found;
}

VBool TrackdControllerInfo::IsButtonPressed(TrackdButtonId buttonId) const
{
	return getButton(buttonId).IsPressed();
}

VBool TrackdControllerInfo::WasButtonToggled(TrackdButtonId buttonId) const
{
	return getButton(buttonId).WasToggled();
}

void TrackdControllerInfo::Setup(void *controllerRef, VUInt32 buttonCount)
{
	mControllerRef = controllerRef;
	for (VUInt32 i = 0; i < buttonCount; i++)
	{
		mButtons.push_back(new TrackdButton((TrackdButtonId)i));
	}
}

void TrackdControllerInfo::UpdateButton(TrackdButtonId buttonId, VBool pressed)
{
	const_cast<TrackdButton&>(getButton(buttonId)).SetPressed(pressed);
}

void TrackdControllerInfo::Update()
{
	for (VUInt32 i = 0; i < mButtons.size(); i++)
	{
		mButtons[i]->Update();
	}
}

}
}
