/* Shows the DICOM data and filter response by using the ImageProcessor class
 * instead of being embedded in the actual demo class. This also makes use of
 * the TextureManager which is designed to make using textures easier. The DICOM
 * data is also downloaded asynchronously, this is so that a loading screen can
 * be displayed while the data is loading (very important for user-friendliness).
 *
 *  Created: 02-Apr-2009
 *   Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "FilterDemo3.h"

// TODO: Put these in a settings file.
#define DICOM_PORT 1234
#define DICOM_HOST "localhost"

using namespace std;
using namespace vimrid::dicom;
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
FilterDemo3::FilterDemo3(VimridSettings &settings) :
	GlutApplication(settings),
	mLoading(true),
	mDicomClient(DICOM_HOST, DICOM_PORT),
	mDicomResult(NULL),
	mTextureManager(),
	mSourceTexture(NULL),
	mSobelDefaultTexture(NULL),
	mSobelInvertedTexture(NULL),
	mSobelScopedTexture(NULL)
{
	Title = "Filter demo 3 - Fri 17 Apr";
	CameraTrim.Z = -5.5;
	EnableCursor = true;
	RenderFps = true;
	RenderCameraPosition = true;
	SetWindowSize(600, 600);

	// Use the DICOM TCP client for downloading DICOM files.
	mDicomClient.SetDownloadCallback(_handleDownload);
}

FilterDemo3::~FilterDemo3()
{
	delete mSourceTexture;
	delete mSobelDefaultTexture;
	delete mSobelInvertedTexture;
	delete mSobelScopedTexture;
	delete mDicomResult;
}

void FilterDemo3::Init()
{
	this->GlutApplication::Init();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	mDicomClient.DownloadAsync(GetSettings().Input);
}

void FilterDemo3::HandleDownload(void *argPtr)
{
	// Use the taskId (argPtr) to get the async result.
	mDicomResult = mDicomClient.GetResult(argPtr);

	/* As we're not in the render loop thread, we should load
	 * the textures as not to cause disruption to the frame rate.
	 * Hopefully GLUT won't have a problem with this...
	 */
	if (mDicomResult->IsSuccessful())
	{
		DicomImage &sourceDicom = *mDicomResult->GetImages()[0];
		ImageMatrix &sourceImage = *sourceDicom.ToImageMatrix();

		ImageMatrix &sourceNormal = *sourceImage.GetNormalisedCopy(0, 255);
		mSourceTexture = new LuminanceTexture(sourceNormal);
		mTextureManager.Add(mSourceTexture);

		// Gets only part of the gradient between the interpolation points.
		ImageProcessor scopedProcessor(sourceImage, SobelOperator());
		scopedProcessor.ScopeMin = 0.5;
		scopedProcessor.ScopeMax = 0.8;

		// Uses the filter in the default way.
		ImageProcessor defaultProcessor(sourceImage, SobelOperator());

		// Inverts the image filter (and so the response).
		ImageProcessor invertedProcessor(sourceImage, SobelOperator());
		invertedProcessor.InvertFilter = true;

		WRITE_DEBUG_TEXT("Processing scoped Sobel...");
		ImageMatrix &sobelScopedImage = *scopedProcessor.ProcessImage();
		mSobelScopedTexture = new LuminanceTexture(sobelScopedImage);
		mTextureManager.Add(mSobelScopedTexture);

		WRITE_DEBUG_TEXT("Processing default Sobel...");
		ImageMatrix &sobelDefaultImage = *defaultProcessor.ProcessImage();
		sobelDefaultImage.PrintDebugFile("/home/nick/tmp/sobel-default-image.out");
		mSobelDefaultTexture = new LuminanceTexture(sobelDefaultImage);
		mTextureManager.Add(mSobelDefaultTexture);

		WRITE_DEBUG_TEXT("Processing inverted Sobel...");
		ImageMatrix &sobelInvertedImage = *invertedProcessor.ProcessImage();
		sobelInvertedImage.PrintDebugFile("/home/nick/tmp/sobel-inverted-image.out");
		mSobelInvertedTexture = new LuminanceTexture(sobelInvertedImage);
		mTextureManager.Add(mSobelInvertedTexture);

		WRITE_DEBUG_TEXT("Image processing complete.");
		mLoading = false;

		delete &sourceImage;
	}
}

void *_handleDownload(void *argPtr)
{
	/* After the async DICOM image download operation has completed,
	 * this static function is fired, so pass the task ID on to the
	 * class where we can get the result back from the client object.
	 */
	((FilterDemo3*)GlutApplication::GetGlutInstance())->HandleDownload(argPtr);
	return NULL;
}

void FilterDemo3::Update()
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

	if (IsCharKeyPressed('q'))
	{
		Exit();
	}

	/* Keep checking for errors on the download, and if one occurs
	 * just throw an exception (should kill the application).
	 */
	if ((mDicomResult != NULL) && !mDicomResult->IsSuccessful())
	{
		THROW_VIMRID_EX(mDicomResult->GetError());
	}

	// Keep checking to see if there are textures that need creating.
	if (mTextureManager.IsCreatePending())
	{
		/* TODO: This creates all textures, even if they've already
		 * been created, is this a bad thing? */
		mTextureManager.CreateAll();
		WRITE_DEBUG_TEXT("Textures were created.");
	}
}

void FilterDemo3::Render()
{
	this->GlutApplication::Render();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glTranslatef(0, 0, GetCameraVector().Z);
	glRotatef(GetCameraVector().Y, 1, 0, 0);
	glRotatef(-GetCameraVector().X, 0, 1, 0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glEnable(GL_TEXTURE_2D);

	if ((mSourceTexture != NULL) && mSourceTexture->IsCreated())
	{
		mSourceTexture->Bind();
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex3f(-2.1, +2.1, 0.0); // top left
		glTexCoord2f(0.0, 1.0); glVertex3f(-2.1, +0.1, 0.0); // bottom left
		glTexCoord2f(1.0, 1.0); glVertex3f(-0.1, +0.1, 0.0); // bottom right
		glTexCoord2f(1.0, 0.0); glVertex3f(-0.1, +2.1, 0.0); // top right
		glEnd();
		mSourceTexture->Unbind();
	}

	if ((mSobelScopedTexture != NULL) && mSobelScopedTexture->IsCreated())
	{
		mSobelScopedTexture->Bind();
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex3f(+0.1, +2.1, 0.0); // top left
		glTexCoord2f(0.0, 1.0); glVertex3f(+0.1, +0.1, 0.0); // bottom left
		glTexCoord2f(1.0, 1.0); glVertex3f(+2.1, +0.1, 0.0); // bottom right
		glTexCoord2f(1.0, 0.0); glVertex3f(+2.1, +2.1, 0.0); // top right
		glEnd();
		mSobelScopedTexture->Unbind();
	}

	if ((mSobelDefaultTexture != NULL) && mSobelDefaultTexture->IsCreated())
	{
		mSobelDefaultTexture->Bind();
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex3f(-2.1, -0.1, 0.0); // top left
		glTexCoord2f(0.0, 1.0); glVertex3f(-2.1, -2.1, 0.0); // bottom left
		glTexCoord2f(1.0, 1.0); glVertex3f(-0.1, -2.1, 0.0); // bottom right
		glTexCoord2f(1.0, 0.0); glVertex3f(-0.1, -0.1, 0.0); // top right
		glEnd();
		mSobelDefaultTexture->Unbind();
	}

	if ((mSobelInvertedTexture != NULL) && mSobelInvertedTexture->IsCreated())
	{
		mSobelInvertedTexture->Bind();
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex3f(+0.1, -0.1, 0.0); // top left
		glTexCoord2f(0.0, 1.0); glVertex3f(+0.1, -2.1, 0.0); // bottom left
		glTexCoord2f(1.0, 1.0); glVertex3f(+2.1, -2.1, 0.0); // bottom right
		glTexCoord2f(1.0, 0.0); glVertex3f(+2.1, -0.1, 0.0); // top right
		glEnd();
		mSobelInvertedTexture->Unbind();
	}

	glDisable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	if (mLoading)
	{
		// Display a crude progress indicator.
		stringstream ss;
		ss << "Loading texture " << (mTextureManager.Count() + 1) << " of 4...";

		glColor3f(0.9, 0.9, 0.9);
		RenderStringOrtho2D(Point2f(10.0, 50.0), GLUT_BITMAP_9_BY_15, ss.str());
	}

	FinishRender();
}

void FilterDemo3::Reshape()
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
