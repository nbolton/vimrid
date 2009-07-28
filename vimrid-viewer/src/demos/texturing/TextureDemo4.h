#ifndef TEXTUREDEMO4_H_
#define TEXTUREDEMO4_H_

#include <vimrid.h>
#include "../../glut/GlutApplication.h"
#include "../../textures/VTextureManager.h"
#include "../../mri/MriVisualiser.h"

namespace vimrid
{
namespace demos
{
namespace texturing
{

class TextureDemo4 : public vimrid::glut::GlutApplication
{
public:
	TextureDemo4(VimridSettings &settings);
	virtual ~TextureDemo4();
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

#endif // TEXTUREDEMO4_H_
