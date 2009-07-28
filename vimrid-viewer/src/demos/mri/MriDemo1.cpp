/* Same as TextureDemo1, but with transparency.
 */

#include "MriDemo1.h"
#include "../../mri/MriSet.h"
#include "../../mri/MriSlice.h"
#include "../../input/DragMode.h"
#include "../../textures/SampleTexture.h"

using namespace std;
using namespace vimrid::textures;
using namespace vimrid::dicom;
using namespace vimrid::imaging;
using namespace vimrid::glut;
using namespace vimrid::mri;
using namespace vimrid::input;

#define DICOM_PORT 1234
//#define DICOM_HOST "home-ws-2-vm-1"
#define DICOM_HOST "localhost"
//#define DICOM_HOST "njbtmp"

#define VIEW_DRAG_MODE 0 // LMB

namespace vimrid
{
namespace demos
{
namespace mri
{

MriDemo1::MriDemo1(VimridSettings &settings) :
	GlutApplication(settings),
	mDicomClient(DICOM_HOST, DICOM_PORT),
	mDicomResult(NULL)
{
	Title = "MRI Demo 1 - Tue 21 Apr";
	EnableCursor = true;
	CameraTrim.X = 30;
	CameraTrim.Y = 40;
	CameraTrim.Z = -4;
	RenderFps = true;
	RenderCameraPosition = true;
	RenderDragInfo = true;
	RenderMouseInfo = true;

	// Use the DICOM TCP client for downloading DICOM files.
	mDicomClient.SetDownloadCallback(_handleDownload);
}

MriDemo1::~MriDemo1()
{
}

// Static method to pass request back to the singleton.
void *MriDemo1::_handleDownload(void *argPtr)
{
	((MriDemo1*)GlutApplication::GetGlutInstance())->handleDownload(argPtr);
	return NULL;
}

void MriDemo1::handleDownload(void *argPtr)
{
	mDicomResult = mDicomClient.GetResult(argPtr);

	if (mDicomResult->IsSuccessful())
	{
		GetMriVisualiser().AddDicomImages(mDicomResult->GetImages());
	}
}

void MriDemo1::Init()
{
	this->GlutApplication::Init();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// check to see if theyre in the right order!
	//GetMriVisualiser().AddTextureAsSlice(new SampleTexture());

	mDicomClient.DownloadAsync(GetSettings().Input);

	/*for (int i = 0; i < 15; i++)
	{
		GetMriVisualiser().AddTextureAsSlice(new SampleTexture());
	}*/
}

void MriDemo1::Reshape()
{
	this->GlutApplication::Reshape();

	glViewport(0, 0, (GLsizei)GetWindowSize().Width, (GLsizei)GetWindowSize().Height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, (GLfloat)GetWindowSize().Width / (GLfloat)GetWindowSize().Height, 1.0, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void MriDemo1::Update()
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

	/* Keep checking for errors on the download, and if one occurs
	 * just throw an exception (should kill the application). */
	if ((mDicomResult != NULL) && !mDicomResult->IsSuccessful())
	{
		THROW_VIMRID_EX(mDicomResult->GetError());
	}

	// Keep checking to see if there are textures that need creating.
	if (GetTextureManager().IsCreatePending())
	{
		GetTextureManager().CreateAll();
		WRITE_DEBUG_TEXT("Textures were created.");
	}
}

void MriDemo1::Render()
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

}
}
}
