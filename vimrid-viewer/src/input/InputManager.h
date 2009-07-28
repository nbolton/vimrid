#ifndef INPUTMANAGER_H_
#define INPUTMANAGER_H_

#include <vimrid.h>
#include <vector>
#include "TrackdSensorInfo.h"
#include "TrackdControllerInfo.h"
#include "TrackdDeviceInfo.h"
#include "MouseInfo.h"

#ifdef USE_TRACKD
#include <trackdAPI.h>
#endif

namespace vimrid
{

class VimridUtility;

namespace input
{

class InputManager
{
public:
	InputManager();
	void InitTrackd(VUInt32 trackerId);
	virtual void Update();
	void UpdateMousePosition(VUInt32 x, VUInt32 y);
	const TrackdSensorInfo &GetTrackdSensorInfo(VUInt32 sensorId) const;
	const MouseInfo &GetMouseInfo() const;
	void AddTrackdSensor(VUInt32 sensorId);
	TrackdDeviceInfo &AddTrackdDevice(VUInt32 sensorId);
	TrackdDeviceInfo &AddTrackdDevice(VUInt32 sensorId, VInt32 controllerId);
	const TrackdDeviceInfo &GetTrackdDevice(VUInt32 sensorId) const;
	VBool IsTrackdSupported() const { return mTrackdSupported; }
	void SetWindowSize(const VSizeF &windowSize) { mWindowSize = windowSize; }
	void UpdateMouseButtons(VBool left, VBool right);
private:
	VBool mTrackdSupported;
	MouseInfo mMouse;
	VSizeF mWindowSize;
private:
#ifdef USE_TRACKD
	void* tracker;
#endif
	std::vector<TrackdDeviceInfo*> mTrackdDevices;
	void updateTrackdDevices();
};

}
}

#endif // INPUTMANAGER_H_
