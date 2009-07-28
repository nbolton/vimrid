#ifndef CUBEDEMO6_H_
#define CUBEDEMO6_H_

#include "../../glut/GlutApplication.h"
#include "TestCube.h"

namespace vimrid
{
namespace demos
{
namespace cubedemos
{

class CubeDemo6 : public vimrid::glut::GlutApplication
{
public:
	CubeDemo6(VimridSettings &settings);

protected:
	void Init();
	void Update();
	void Render();
	void Reshape();

private:
	void drawStereoEyeView(GLenum eyeBuffer);
	void drawGears();
	void createGear(
			float innerRadius,
			float outerRadius,
			float width,
			int teeth,
			float toothDepth);
	float viewRotateX, viewRotateY, viewRotateZ;
	int gear1, gear2, gear3;
	float gearRotateAngle;
	float stereoEyeSeparation;
	float fixationPoint;
	float aspectRatio, frustumNearClip, frustumFarClip;
	static const int maxRotateVelocity = 20;
	float viewRotateXVelocity;
	float viewRotateYVelocity;
};

}
}
}

#endif // CUBEDEMO6_H_
