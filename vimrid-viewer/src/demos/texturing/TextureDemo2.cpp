/* Same as TextureDemo1, but with transparency.
 */

#include "TextureDemo2.h"

using namespace std;
using namespace vimrid::input;

namespace vimrid
{
namespace demos
{
namespace texturing
{

TextureDemo2::TextureDemo2(VimridSettings &settings)
	: GlutApplication(settings)
{
	Title = "Texture demo 2 - Tue 14 Apr 11:35";
	EnableCursor = true;
	CameraTrim.Z = -4;
}

TextureDemo2::~TextureDemo2()
{
}

void TextureDemo2::Init()
{
	this->GlutApplication::Init();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	//glEnable(GL_DEPTH_TEST); // TODO: James said disabling this may help?

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, textureIds);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	int i, j;
	GLubyte pixel;
	for (i = 0; i < TEXTURE_HEIGHT; i++)
	{
		for (j = 0; j < TEXTURE_WIDTH; j++)
		{
			pixel = ((((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255);
			texture[i][j][0] = pixel;
			texture[i][j][1] = pixel;
			texture[i][j][2] = pixel;
			texture[i][j][3] = 200;
		}
	}

	glBindTexture(GL_TEXTURE_2D, textureIds[0]);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA,
		TEXTURE_WIDTH, TEXTURE_HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, texture);
}

void TextureDemo2::Reshape()
{
	this->GlutApplication::Reshape();

	glViewport(0, 0, (GLsizei)GetWindowSize().Width, (GLsizei)GetWindowSize().Height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, (GLfloat)GetWindowSize().Width / (GLfloat)GetWindowSize().Height, 1.0, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void TextureDemo2::Update()
{
	this->GlutApplication::Update();

	const MouseInfo &mouse = GetInputManager()->GetMouseInfo();

	// When mouse moves up and down, move toward/away from screen.
	CameraPoint.X = (((GetWindowSize().Width / 2) * -1) + mouse.GetPosition().X) * 0.15;
	CameraPoint.Y = (((GetWindowSize().Height / 2) * -1) + mouse.GetPosition().Y) * -0.2;

	if (IsSpecialKeyPressed(GLUT_KEY_UP))
	{
		CameraPoint.Z += .01;
	}

	if (IsSpecialKeyPressed(GLUT_KEY_DOWN))
	{
		CameraPoint.Z -= .01;
	}
}

void TextureDemo2::Render()
{
	this->GlutApplication::Render();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);

	// Use model view so that rotation value is literal, not added.
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	Point3f trim = CameraTrim;
	Point3f point = CameraPoint;

	glTranslatef(0, 0, CameraTrim.Z + CameraPoint.Z);
	glRotatef(CameraTrim.Y + CameraPoint.Y, 1, 0, 0);
	glRotatef(-(CameraTrim.X + CameraPoint.X), 0, 1, 0);

	glBindTexture(GL_TEXTURE_2D, textureIds[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, +1.0, -1.0); // top left
	glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, -1.0, -1.0); // bottom left
	glTexCoord2f(1.0, 1.0); glVertex3f(+1.5, -1.0, -1.0); // bottom right
	glTexCoord2f(1.0, 0.0); glVertex3f(+1.5, +1.0, -1.0); // top right
	glEnd();

	glBindTexture(GL_TEXTURE_2D, textureIds[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, +1.0, 0.0); // top left
	glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, -1.0, 0.0); // bottom left
	glTexCoord2f(1.0, 1.0); glVertex3f(+1.0, -1.0, 0.0); // bottom right
	glTexCoord2f(1.0, 0.0); glVertex3f(+1.0, +1.0, 0.0); // top right
	glEnd();

	glFlush();
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();

FinishRender();
}

}
}
}
