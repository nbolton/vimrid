/* Same as TextureDemo1, but with transparency.
 */

#include "TextureDemo4.h"
#include "../../mri/MriSet.h"
#include "../../mri/MriSlice.h"

using namespace std;
using namespace vimrid::textures;
using namespace vimrid::dicom;
using namespace vimrid::imaging;
using namespace vimrid::glut;
using namespace vimrid::mri;
using namespace vimrid::input;

namespace vimrid
{
namespace demos
{
namespace texturing
{

TextureDemo4::TextureDemo4(VimridSettings &settings) :
	GlutApplication(settings),
	mTexture(NULL)
{
	Title = "Texture demo 4 - Mon 20 Apr";
	EnableCursor = true;
	CameraTrim.X = 30;
	CameraTrim.Y = 40;
	CameraTrim.Z = -4;
	RenderFps = true;
}

TextureDemo4::~TextureDemo4()
{
	delete mTexture;
}

void TextureDemo4::Init()
{
	this->GlutApplication::Init();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);

	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int i, j;
	GLubyte pixel;
	ImageMatrix sourceImage(64, 64, 4);
	for (i = 0; i < 64; i++)
	{
		for (j = 0; j < 64; j++)
		{
			pixel = ((((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255);
			/*sourceImage[i][j][0] = pixel;
			sourceImage[i][j][1] = pixel;
			sourceImage[i][j][2] = pixel;
			sourceImage[i][j][3] = 200;*/

			sourceImage.Get(i, j, 0) = pixel;
			sourceImage.Get(i, j, 1) = pixel;
			sourceImage.Get(i, j, 2) = pixel;
			sourceImage.Get(i, j, 3) = 200;
		}
	}

	mTexture = new VTexture(sourceImage, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	mTextureManager.Add(mTexture);
	mTextureManager.CreateAll();

	GetMriVisualiser().AddTextureAsSlice(mTexture);
	GetMriVisualiser().AddTextureAsSlice(mTexture);
	GetMriVisualiser().AddTextureAsSlice(mTexture);
	GetMriVisualiser().AddTextureAsSlice(mTexture);
	GetMriVisualiser().AddTextureAsSlice(mTexture);
	GetMriVisualiser().AddTextureAsSlice(mTexture);
	GetMriVisualiser().AddTextureAsSlice(mTexture);
	GetMriVisualiser().AddTextureAsSlice(mTexture);
	GetMriVisualiser().AddTextureAsSlice(mTexture);
	GetMriVisualiser().AddTextureAsSlice(mTexture);
}

void TextureDemo4::Reshape()
{
	this->GlutApplication::Reshape();

	glViewport(0, 0, (GLsizei)GetWindowSize().Width, (GLsizei)GetWindowSize().Height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, (GLfloat)GetWindowSize().Width / (GLfloat)GetWindowSize().Height, 1.0, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void TextureDemo4::Update()
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

	UpdateSprites();
	GetMriVisualiser().Update();
}

void TextureDemo4::Render()
{
	this->GlutApplication::Render();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glTranslatef(0, 0, CameraTrim.Z + CameraPoint.Z);
	glRotatef(CameraTrim.Y + CameraPoint.Y, 1, 0, 0);
	glRotatef(-(CameraTrim.X + CameraPoint.X), 0, 1, 0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	RenderSprites();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	FinishRender();
}

}
}
}
