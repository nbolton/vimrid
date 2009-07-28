#ifndef CUBEDEMO8_H_
#define CUBEDEMO8_H_

#include "../../glut/GlutApplication.h"
#include "TestCube.h"

namespace vimrid
{
namespace demos
{
namespace cubedemos
{

class CubeDemo8 : public vimrid::glut::GlutApplication
{
public:
	CubeDemo8(VimridSettings &settings);

protected:
	void Init();
	void Render();
	void Update();
	void Reshape();

private:
	void drawStereoEyeView(GLenum eyeBuffer);
	void drawCube();
	void createCube();
	void updateCamera();
	float stereoEyeSeparation;
	float fixationPoint;
	float aspectRatio;
	float frustumNearClip;
	float frustumFarClip;
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

#endif // CUBEDEMO8_H_
