#ifndef CUBEDEMO7_H_
#define CUBEDEMO7_H_

#include "../../glut/GlutApplication.h"
#include "TestCube.h"

namespace vimrid
{
namespace demos
{
namespace cubedemos
{

class CubeDemo7 : public vimrid::glut::GlutApplication
{
public:
	CubeDemo7(VimridSettings &settings);

protected:
	void Init();
	void Update();
	void Render();
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

#endif //VCUBEDEMO7_H_
