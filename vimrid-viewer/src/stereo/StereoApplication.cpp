/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 07-May-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "StereoApplication.h"
#include <GL/gl.h>

namespace vimrid
{
namespace stereo
{

StereoApplication::StereoApplication(
	VFloat frustumNearClip,
	VFloat frustumFarClip,
	VFloat fixationPoint,
	VFloat eyeSeparation)
:
	mFrustumNearClip(frustumNearClip),
	mFrustumFarClip(frustumFarClip),
	mFixationPoint(fixationPoint),
	mEyeSeparation(eyeSeparation)
{
}

StereoApplication::~StereoApplication()
{
}

/* Sets up a stereographic projection for a specific eye. The code is
 * loosely based on the glxgears stereo code.
 */
void StereoApplication::RenderStereoBuffer(GLenum eyeBuffer)
{
	// Store so caller knows what the current mode is.
	mCurrentEyeBuffer = eyeBuffer;

	/* Base size of the frustum (anything other than 5, makes
	 * the frustum appear skewed. */
	const VFloat fs = 5.0;

	// The frustum width offset is used in oposing directions for each eye.
	const VFloat fwOffset = 0.5;

	// This allows the aspect ratio to be modified, but should really always be 1.0.
	const VFloat arModifier = 1.0;

	// Frustum width multiplier to be used for both left and right.
	VFloat fw = mFixationPoint * (arModifier / fs);

	/* These could be altered when the window is resized, however there is
	 * no performance hit when run in this class, so it makes more sense to
	 * run them here, so that it's more obvious how the frustum is generated. */
	VDouble frustumLeft = -fs * ((fw - fwOffset * mEyeSeparation) / mFixationPoint);
	VDouble frustumRight = +fs * ((fw + fwOffset * mEyeSeparation) / mFixationPoint);

	/* Each left and right eye must be displayed on either side
	 * of the central view, so where right is -1, left should be +1 */
	VInt32 eyeSide = (eyeBuffer == GL_BACK_LEFT) ? +1 : -1;

	// Tell OpenGL to use the apropriate back buffer (left or right).
	glDrawBuffer(eyeBuffer);

	// Start with a new projection matrix for each eye.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	/* The left and right frustums are swapped for each eye;
	 * if this didn't happen, the viewer would be looking at the
	 * objects from the back (in the left eye) and the front (from
	 * the right eye). This applies to the right eye's frustum, as
	 * you can see the frustumRight variable is past to the first
	 * parameter (which is actually called left) - this could be
	 * misinterprated as a mistake, as it seems counter-intuative
	 * at first glance.
	 */
	switch(eyeBuffer)
	{
		case GL_BACK_LEFT:
			glFrustum(
				(frustumLeft * eyeSide),
				(frustumRight * eyeSide),
				-GetAspectRatio(),
				GetAspectRatio(),
				mFrustumNearClip,
				mFrustumFarClip);
			break;

		case GL_BACK_RIGHT:
			glFrustum(
				(frustumRight * eyeSide),
				(frustumLeft * eyeSide),
				-GetAspectRatio(),
				GetAspectRatio(),
				mFrustumNearClip,
				mFrustumFarClip);
			break;
	}

	/* After the frustum for either eye has been set up, we now need to
	 * move each eye in the same direction of the frustum so that the frustum
	 * planes of each eye are properly aligned. */
	glTranslated((fwOffset * eyeSide) * mEyeSeparation, 0.0, 0.0);

	// Now render as normal (pure virtual function).
	RenderDelegate();
}

}
}
