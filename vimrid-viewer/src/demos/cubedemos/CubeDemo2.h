#ifndef CUBEDEMO2_H_
#define CUBEDEMO2_H_

#include "../../glut/GlutApplication.h"
#include "TestCube.h"

namespace vimrid
{
namespace demos
{
namespace cubedemos
{

class CubeDemo2 : public vimrid::glut::GlutApplication
{
public:
	CubeDemo2(VimridSettings &settings);

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
	void drawCube(unsigned int buffer, float depthPerception);
	void updateCamera();
	TestCube cube;
	int a,b;
	unsigned int currentVer;
};

}
}
}

#endif // CUBEDEMO2_H_
