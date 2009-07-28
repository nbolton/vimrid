#ifndef TEXTUREDEMO1_H_
#define TEXTUREDEMO1_H_

#include "../../glut/GlutApplication.h"

#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 64

namespace vimrid
{
namespace demos
{
namespace texturing
{

class TextureDemo1 : public vimrid::glut::GlutApplication
{
public:
	TextureDemo1(VimridSettings &settings);
	virtual ~TextureDemo1();
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


#endif // TEXTUREDEMO1_H_
