#ifndef TEXTUREDEMO3_H_
#define TEXTUREDEMO3_H_

#include <vimrid.h>
#include "../../glut/GlutApplication.h"
#include "../../textures/VTextureManager.h"

namespace vimrid
{
namespace demos
{
namespace texturing
{

class TextureDemo3 : public vimrid::glut::GlutApplication
{
public:
	TextureDemo3(VimridSettings &settings);
	virtual ~TextureDemo3();
	void Init();
	void Update();
	void Render();
	void Reshape();
private:
	vimrid::textures::VTextureManager mTextureManager;
	vimrid::textures::VTexture *mTexture;
};

}
}
}

#endif // TEXTUREDEMO3_H_
