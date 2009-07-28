/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 22-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef TRACKDSENSORINFO_H_
#define TRACKDSENSORINFO_H_

#include <vimrid.h>

namespace vimrid
{
namespace input
{

class TrackdSensorInfo
{
public:
	TrackdSensorInfo(VUInt32 sensorId);
	virtual ~TrackdSensorInfo();
	VUInt32 GetSensorId() const { return mSensorId; }
	const VPoint3f &GetPosition() const { return mPosition; }
	void Update(float x, float y, float z);
private:
	VUInt32 mSensorId;
	VPoint3f mPosition;
};

}
}

#endif // TRACKDSENSORINFO_H_
