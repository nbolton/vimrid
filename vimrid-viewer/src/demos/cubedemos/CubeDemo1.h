#ifndef CUBEDEMO1_H_
#define CUBEDEMO1_H_

#include "../../glut/GlutApplication.h"
#include "TestCube.h"

namespace vimrid
{
namespace demos
{
namespace cubedemos
{

class CubeDemo1 : public vimrid::glut::GlutApplication
{
public:
	CubeDemo1(VimridSettings &settings);

protected:
	void Init();
	void Update();
	void Render();
	void Resized();
	void KeyCharPressed(unsigned char key);
	void KeySpecialDown(int key);
	void KeySpecialUp(int key);
	void MouseClicked(int button, int state);
	void MouseMoved();

private:
	void createTestCube();
	void drawCube(float translateX, float translateY, float translateZ,
			float rotateX, float rotateY, float rotateZ);
	void updateCamera();
	TestCube cube;
	int a,b;
	unsigned int currentVer;
};

}
}
}

#endif // CUBEDEMO1_H_
