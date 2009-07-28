/* This demo is created to show how image filtering works on a small circle
 * without anti-aliasing.
 *
 *  Created: 02-Apr-2009
 *   Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "FilterDemo4.h"
#include "../../input/DragMode.h"

// TODO: Put these in a settings file.
#define DICOM_PORT 1234
#define DICOM_HOST "localhost"

#define VIEW_DRAG_MODE 0 // LMB

using namespace std;
using namespace vimrid::glut;
using namespace vimrid::imaging;
using namespace vimrid::imaging::processing;
using namespace vimrid::textures;
using namespace vimrid::input;

namespace vimrid
{
namespace demos
{
namespace filterdemos
{

// Result won't be set until we get a response from the server.
// Textures won't be loaded until we have a non-null DICOM result.
FilterDemo4::FilterDemo4(VimridSettings &settings) :
	GlutApplication(settings),
	mLoading(true)
{
	Title = "Filter demo 4 - Tue 28 Apr";
	CameraTrim.Z = -5.5;
	CameraTrim.X = 30;
	CameraTrim.Y = 40;
	EnableCursor = true;
	RenderFps = true;
	RenderCameraPosition = true;
	SetWindowSize(600, 600);
}

FilterDemo4::~FilterDemo4()
{
}

void FilterDemo4::Init()
{
	this->GlutApplication::Init();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	VInt32 imageArray[9][9] =
	{
		{ 0,  0,  0,  0,  0,  0,  0,  0,  0 },
        { 0,  0,  0,  0,  0,  0,  0,  0,  0 },
        { 0,  0,  0,  10, 10, 10, 0,  0,  0 },
        { 0,  0,  10, 10, 10, 10, 10, 0,  0 },
        { 0,  0,  10, 10, 10, 10, 10, 0,  0 },
        { 0,  0,  10, 10, 10, 10, 10, 0,  0 },
        { 0,  0,  0,  10, 10, 10, 0,  0,  0 },
        { 0,  0,  0,  0,  0,  0,  0,  0,  0 },
        { 0,  0,  0,  0,  0,  0,  0,  0,  0 }
	};

	ImageMatrix *sourceImage = new ImageMatrix(9, 9, 1);
	sourceImage->LoadPixels(*imageArray);

	// Uses the filter in the default way.
	ImageProcessor defaultProcessor(*sourceImage, SobelOperator());
	defaultProcessor.FilterRotations = 2;

	// Inverts the image filter (and so the response).
	ImageProcessor invertedProcessor(*sourceImage, SobelOperator());
	invertedProcessor.InvertFilter = true;

	// Gets only part of the gradient between the interpolation points.
	ImageProcessor scopedProcessor(*sourceImage, SobelOperator());
	scopedProcessor.ScopeMin = 0.5;
	scopedProcessor.ScopeMax = 0.8;

	WRITE_DEBUG_TEXT("Printing filter...");
	SobelOperator().PrintDebugView();

	WRITE_DEBUG_TEXT("Adding original texture...");
	GetMriVisualiser().AddTextureAsSlice(new LuminanceTexture(*sourceImage));

	WRITE_DEBUG_TEXT("Processing default Sobel...");
	ImageMatrix &sobelDefaultImage = *defaultProcessor.ProcessImage();
	GetMriVisualiser().AddTextureAsSlice(new LuminanceTexture(sobelDefaultImage));

	WRITE_DEBUG_TEXT("Processing inverted Sobel...");
	ImageMatrix &sobelInvertedImage = *invertedProcessor.ProcessImage();
	GetMriVisualiser().AddTextureAsSlice(new LuminanceTexture(sobelInvertedImage));

	WRITE_DEBUG_TEXT("Processing scoped Sobel...");
	ImageMatrix &sobelScopedImage = *scopedProcessor.ProcessImage();
	GetMriVisualiser().AddTextureAsSlice(new LuminanceTexture(sobelScopedImage));

	WRITE_DEBUG_TEXT("Image download complete.");
	mLoading = false;
}

void FilterDemo4::Update()
{
	this->GlutApplication::Update();

	CameraPoint = GetDragController().GetMode(VIEW_DRAG_MODE).GetDragVector() * 0.2;

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

	// Keep checking to see if there are textures that need creating.
	if (GetTextureManager().IsCreatePending())
	{
		GetTextureManager().CreateAll();
		WRITE_DEBUG_TEXT("Textures were created.");
	}
}

void FilterDemo4::Render()
{
	this->GlutApplication::Render();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glTranslatef(0, 0, GetCameraVector().Z);
	glRotatef(GetCameraVector().Y, 1, 0, 0);
	glRotatef(GetCameraVector().X, 0, 1, 0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// Make all sprites transparent.
	glColor4f(1, 1, 1, .7);

	RenderSprites();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	FinishRender();
}

void FilterDemo4::Reshape()
{
	this->GlutApplication::Reshape();

	glViewport(0, 0, (GLsizei)GetWindowSize().Width, (GLsizei)GetWindowSize().Height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, (GLfloat)GetWindowSize().Width / (GLfloat)GetWindowSize().Height, 1.0, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

}
}
}
