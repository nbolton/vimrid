/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 07-May-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef STEREOAPPLICATION_H_
#define STEREOAPPLICATION_H_

#include <GL/gl.h>
#include <vimrid.h>

namespace vimrid
{
namespace stereo
{

class StereoApplication
{
public:
	StereoApplication(
		VFloat frustumNearClip,
		VFloat frustumFarClip,
		VFloat fixationPoint,
		VFloat eyeSeparation);
	virtual ~StereoApplication();
protected:
	void RenderStereoBuffer(GLenum eyeBuffer);
	virtual void RenderDelegate() = 0;
	VFloat &GetFrustumNearClip() { return mFrustumNearClip; }
	VFloat &GetFrustumFarClip() { return mFrustumFarClip; }
	VFloat &GetFixationPoint() { return mFixationPoint; }
	VFloat &GetEyeSeparation() { return mEyeSeparation; }
	virtual VFloat GetAspectRatio() const = 0;
	const GLenum &GetCurrentEyeBuffer() const { return mCurrentEyeBuffer; }
private:
	VFloat mFrustumNearClip;
	VFloat mFrustumFarClip;
	VFloat mFixationPoint;
	VFloat mEyeSeparation;
	GLenum mCurrentEyeBuffer;
};

}
}

#endif // STEREOAPPLICATION_H_
