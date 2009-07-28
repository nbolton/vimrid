#ifndef CUBEDEMO5_H_
#define CUBEDEMO5_H_

#include "../../sdl/SdlApplication.h"
#include "TestCube.h"

namespace vimrid
{
namespace demos
{
namespace cubedemos
{

class CubeDemo5 : public vimrid::sdl::SdlApplication
{
public:
	CubeDemo5(VimridSettings &settings);

protected:
	void Init();
	void Update();
	void Render();
	void Reshape();

private:
	void drawGears();
	void createGear(
			GLfloat innerRadius,
			GLfloat outerRadius,
			GLfloat width,
			GLint teeth,
			GLfloat toothDepth);
	GLfloat viewRotateX, viewRotateY, viewRotateZ;
	GLint gear1, gear2, gear3;
	GLfloat angle;
	GLfloat eyesep;		/* Eye separation. */
	GLfloat fix_point;	/* Fixation point distance.  */
	GLfloat left, right, asp;	/* Stereo frustum params.  */
	static const int maxRotateVelocity = 60;
	float viewRotateXVelocity;
	float viewRotateYVelocity;
};

}
}
}

#endif // CUBEDEMO5_H_
