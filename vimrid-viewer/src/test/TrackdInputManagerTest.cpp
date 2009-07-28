/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 25-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "TrackdInputManagerTest.h"
#include "../input/TrackdControllerInfo.h"
#include "../input/TrackdDeviceInfo.h"
#include "../input/TrackdSensorInfo.h"
#include <unistd.h>
#include <vimrid.h>
#include <string>
#include "../VimridUtility.h"

#define TD_TRACKER_KEY 4126
#define TD_WAND_CONTROLLER_KEY 4127
#define TD_GLASSES_ID 0
#define TD_WAND_ID 1

using namespace std;
using namespace vimrid::input;

namespace vimrid
{
namespace test
{

TrackdInputManagerTest::TrackdInputManagerTest() :
	mGlasses(NULL),
	mWand(NULL)
{
}

TrackdInputManagerTest::~TrackdInputManagerTest()
{
}

void TrackdInputManagerTest::Run()
{
	if (!mInputManager.IsTrackdSupported())
	{
		THROW_VIMRID_EX("This binary was not built with trackd support.");
	}

	mInputManager.InitTrackd(TD_TRACKER_KEY);
	mGlasses = &mInputManager.AddTrackdDevice(TD_GLASSES_ID);
	mWand = &mInputManager.AddTrackdDevice(TD_WAND_ID, TD_WAND_CONTROLLER_KEY);

	while (true)
	{
		mInputManager.Update();
		VPoint3f gp = mGlasses->GetSensor().GetPosition();
		VPoint3f wp = mWand->GetSensor().GetPosition();

		cout.setf(ios::showpos);
		cout.setf(ios_base::fixed, ios_base::floatfield);
		cout.precision(2);

		VBool b1 = mWand->GetController().IsButtonPressed(TRACKD_BUTTON_1);
		VBool b2 = mWand->GetController().IsButtonPressed(TRACKD_BUTTON_2);
		VBool b3 = mWand->GetController().IsButtonPressed(TRACKD_BUTTON_3);

		cout << "[Glasses: " << gp << "] "
			<< "[Wand: " << wp
			<< " B1: " << (b1 ? "On" : "Off")
			<< " B2: " << (b2 ? "On" : "Off")
			<< " B3: " << (b3 ? "On" : "Off")
			<< "]" << endl;

		sleep(1);
	}
}

}
}
