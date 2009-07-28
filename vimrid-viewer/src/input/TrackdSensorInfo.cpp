/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 22-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "TrackdSensorInfo.h"

namespace vimrid
{
namespace input
{

TrackdSensorInfo::TrackdSensorInfo(VUInt32 sensorId)
	: mSensorId(sensorId)
{
}

TrackdSensorInfo::~TrackdSensorInfo()
{
}

void TrackdSensorInfo::Update(float x, float y, float z)
{
	mPosition.X = x;
	mPosition.Y = y;
	mPosition.Z = z;
}

}
}
