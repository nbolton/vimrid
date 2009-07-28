#ifndef CUBEDEMO4_H_
#define CUBEDEMO4_H_

#include "../../glut/GlutApplication.h"
#include "TestCube.h"

namespace vimrid
{
namespace demos
{
namespace cubedemos
{

class CubeDemo4 : public vimrid::glut::GlutApplication
{
public:
	CubeDemo4(VimridSettings &settings);

protected:
	void Init();
	void Update();
	void Render();
	void Resized();

private:
	void createTestCube();
	void drawCube(GLenum buffer);
	void updateCamera();
	TestCube cube;
	int a,b;
	unsigned int currentVer;
	const vimrid::input::TrackdDeviceInfo *glasses, *wand;
	const vimrid::input::MouseInfo *mouse;
	GLfloat stereoOffsetAdjust, stereoBalanceAdjust;
	Point3f wandDragFrom;
	Point3f lastWandPoint;
	bool wandDragActive;
};

}
}
}

#endif // CUBEDEMO4_H_
