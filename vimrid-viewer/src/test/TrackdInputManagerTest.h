/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 25-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef TRACKDINPUTMANAGERTEST_H_
#define TRACKDINPUTMANAGERTEST_H_

#include "../input/InputManager.h"

namespace vimrid
{
namespace input
{
class TrackdDeviceInfo;
}
namespace test
{

class TrackdInputManagerTest
{
public:
	TrackdInputManagerTest();
	virtual ~TrackdInputManagerTest();
	void Run();
private:
	vimrid::input::InputManager mInputManager;
	vimrid::input::TrackdDeviceInfo *mGlasses;
	vimrid::input::TrackdDeviceInfo *mWand;
};

}
}

#endif // TRACKDINPUTMANAGERTEST_H_
