#include "InputManager.h"
#include <iostream>
#include "../VimridUtility.h"

using namespace std;

namespace vimrid
{
namespace input
{

InputManager::InputManager()
:
	mTrackdSupported(false)
{
#ifdef USE_TRACKD
	mTrackdSupported = true;
#endif
}

void InputManager::AddTrackdSensor(VUInt32 sensorId)
{
	AddTrackdDevice(sensorId);
}

TrackdDeviceInfo &InputManager::AddTrackdDevice(VUInt32 sensorId)
{
	return AddTrackdDevice(sensorId, -1);
}

TrackdDeviceInfo &InputManager::AddTrackdDevice(VUInt32 sensorId, VInt32 controllerId)
{
	if (!mTrackdSupported)
	{
		THROW_VIMRID_EX("Cannot add a device when trackd is not supported.");
	}

	TrackdDeviceInfo *device = new TrackdDeviceInfo(sensorId);

	// When -1 is passed, assume not required.
	if (controllerId != -1)
	{
#ifdef USE_TRACKD
		void *ref = initController(controllerId);
		if (!ref)
		{
			THROW_VIMRID_EX("Unable to initialise specified trackd controller.");
		}
		device->mController.Setup(ref, getNumButtons(ref));
#endif
	}

	mTrackdDevices.push_back(device);

	return *device;
}

const TrackdDeviceInfo &InputManager::GetTrackdDevice(VUInt32 sensorId) const
{
	if (mTrackdSupported)
	{
		TrackdDeviceInfo *found = NULL;
		for (VUInt32 i = 0; i < mTrackdDevices.size(); i++)
		{
			if (mTrackdDevices[i]->GetSensor().GetSensorId() == sensorId)
			{
				found = mTrackdDevices[i];
				break;
			}
		}

		if (found == NULL)
		{
			THROW_VIMRID_EX("Trackd device with Sensor ID " <<
				sensorId << " does not exist.");
		}

		return *found;
	}
	else
	{
		THROW_VIMRID_EX("Cannot get device info when trackd is not supported.");
	}
}

void InputManager::InitTrackd(VUInt32 trackerId)
{
	if (!mTrackdSupported)
	{
		THROW_VIMRID_EX("Cannot setup trackd when it is not supported.");
	}

#ifdef USE_TRACKD
	tracker = initTracker(trackerId);
#endif
}

const MouseInfo &InputManager::GetMouseInfo() const
{
	return mMouse;
}

void InputManager::Update()
{
	updateTrackdDevices();
	mMouse.Update();
}

void InputManager::updateTrackdDevices()
{
#ifdef USE_TRACKD
	// di = Device index.
	for (VUInt32 di = 0; di < mTrackdDevices.size(); di++)
	{
		if (mTrackdDevices[di])
		{
			TrackdDeviceInfo &device = *mTrackdDevices[di];
			TrackerSensor &native = *getSensor(tracker, device.GetSensor().GetSensorId());

			device.mSensor.Update(native.x, -native.z, -native.y);

			// Only some devices have controllers.
			if (device.HasController())
			{
				void* ref = device.mController.GetControllerRef();

				// bi = Button index.
				for (VUInt32 bi = 0; bi < device.mController.GetButtonCount(); bi++)
				{
					// For each button, set the press state.
					device.mController.UpdateButton(
						(TrackdButtonId)bi, getButton(ref, bi));
				}
				device.mController.Update();
			}
		}
	}
#endif //USE_TRACKD
}

void InputManager::UpdateMouseButtons(VBool left, VBool right)
{
	mMouse.UpdateButtons(left, right);
}

void InputManager::UpdateMousePosition(VUInt32 x, VUInt32 y)
{
	if ((x >= 0) && (x <= mWindowSize.Width))
	{
		mMouse.UpdateX(x);
	}

	if ((y >= 0) && (y <= mWindowSize.Height))
	{
		mMouse.UpdateY(y);
	}
}

const TrackdSensorInfo &InputManager::GetTrackdSensorInfo(VUInt32 sensorId) const
{
	if (mTrackdSupported)
	{
		const TrackdSensorInfo *found = NULL;
		for (VUInt32 i = 0; i < mTrackdDevices.size(); i++)
		{
			if (mTrackdDevices[i]->GetSensor().GetSensorId() == sensorId)
			{
				found = &mTrackdDevices[i]->GetSensor();
			}
		}

		if (!found)
		{
			THROW_VIMRID_EX("Sensor does not exist with specified ID.");
		}

		return *found;
	}
	else
	{
		THROW_VIMRID_EX("Cannot get sensor info when trackd is not supported.");
	}
}

}
}
