#ifndef FILTERDEMO4_H_
#define FILTERDEMO4_H_

#include "../../glut/GlutApplication.h"
#include "../cubedemos/TestCube.h"
#include "../../textures/LuminanceTexture.h"
#include "../../textures/VTextureManager.h"

namespace vimrid
{
namespace demos
{
namespace filterdemos
{

class FilterDemo4: public vimrid::glut::GlutApplication
{
public:
	FilterDemo4(VimridSettings &settings);
	virtual ~FilterDemo4();
	void HandleDownload(void *argPtr);
protected:
	void Init();
	void Update();
	void Render();
	void Reshape();
private:
	VBool mLoading;
};

void *_handleDownload(void *argPtr);

}
}
}

#endif // FILTERDEMO4_H_
