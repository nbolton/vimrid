#ifndef CUBEDEMO9_H_
#define CUBEDEMO9_H_

#include "../../glut/GlutApplication.h"
#include "../../stereo/StereoApplication.h"
#include "TestCube.h"

namespace vimrid
{
namespace demos
{
namespace cubedemos
{

class CubeDemo9 :
	public vimrid::glut::GlutApplication,
	public vimrid::stereo::StereoApplication
{
public:
	CubeDemo9(VimridSettings &settings);
protected:
	void Init();
	void Render();
	void Update();
	void Reshape();
	virtual VFloat GetAspectRatio() const;
	virtual void RenderDelegate();
private:
	//void drawStereoEyeView(GLenum eyeBuffer);
	void drawCube();
	void createCube();
	void updateCamera();
	/*float stereoEyeSeparation;
	float fixationPoint;
	float aspectRatio;
	float frustumNearClip;
	float frustumFarClip;*/
	TestCube cube;
	int a,b;
	unsigned int currentVer;
	vimrid::input::TrackdDeviceInfo *glasses, *wand;
	const vimrid::input::MouseInfo *mouse;
	Point3f wandDragFrom;
	Point3f lastWandPoint;
	bool wandDragActive;
	bool enableWandDrag;
};

}
}
}

#endif // CUBEDEMO9_H_
