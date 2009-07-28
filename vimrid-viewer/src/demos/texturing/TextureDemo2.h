#ifndef TEXTUREDEMO2_H_
#define TEXTUREDEMO2_H_

#include "../../glut/GlutApplication.h"

#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 64

namespace vimrid
{
namespace demos
{
namespace texturing
{

class TextureDemo2 : public vimrid::glut::GlutApplication
{
public:
	TextureDemo2(VimridSettings &settings);
	virtual ~TextureDemo2();
	void Init();
	void Update();
	void Render();
	void Reshape();
private:
	GLuint textureIds[1];
	GLubyte texture[TEXTURE_HEIGHT][TEXTURE_WIDTH][4];
};

}
}
}

#endif // TEXTUREDEMO2_H_
