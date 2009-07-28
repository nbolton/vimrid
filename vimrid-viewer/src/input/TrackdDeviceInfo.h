/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 15-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef TRACKDEVICEINFO_H_
#define TRACKDEVICEINFO_H_

#include "TrackdSensorInfo.h"
#include "TrackdControllerInfo.h"

namespace vimrid
{
namespace input
{
class InputManager;
class TrackdDeviceInfo
{
friend class InputManager;
public:
	TrackdDeviceInfo(VUInt32 sensorId);
	virtual ~TrackdDeviceInfo();
	const TrackdSensorInfo &GetSensor() const { return mSensor; }
	const TrackdControllerInfo &GetController() const { return mController; }
	VBool HasController() { return mController.GetControllerRef() != NULL; }
private:
	TrackdSensorInfo mSensor;
	TrackdControllerInfo mController;
};

}
}

#endif // TRACKDEVICEINFO_H_
