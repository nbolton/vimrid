/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 07-May-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "VimridViewer.h"
#include "../input/DragMode.h"
#include "../textures/DarkSampleTexture.h"
#include "../VimridMenu.h"
#include "../ui/Label.h"
#include "../ui/Button.h"
#include <vector>
#include <typeinfo>
#include "../textures/SampleTexture.h"
#include "../mri/ScrollController.h"

#define VIEW_DRAG_MODE 0 // LMB

#define DICOM_PORT 1234
//#define DICOM_HOST "home-ws-2-vm-1"
#define DICOM_HOST "localhost"
//#define DICOM_HOST "njbtmp"

using namespace std;
using namespace vimrid::textures;
using namespace vimrid::ui;
using namespace vimrid::dicom;
using namespace vimrid::imaging;
using namespace vimrid::imaging::processing;
using namespace vimrid::mri;

namespace vimrid
{
namespace viewer
{

VimridViewer::VimridViewer(
	VimridSettings &settings)
	:
	GlutApplication(settings),
	//StereoApplication(5.0, 60.0, 45.0, 5.5) // original from cube demos
	//StereoApplication(5.0, 60.0, 12.75, 1.35), // initial values (broken matrix)
	StereoApplication(5.0, 60.0, 15, 0.3), // best values for see3d theatre
	mDicomClient(DICOM_HOST, DICOM_PORT),
	mDicomResult(NULL),
	mDicomLoadingScreen(NULL),
	mMainMenu(NULL),
	mCameraPushedBack(false),
	mFilterMode(VV_FM_None),
	mDicomLoadCancel(false),
	mAlphaEnabled(true),
	mMenuAutoShown(false)
{
	Title = "ViMRID Viewer";
	EnableCursor = false;
	RenderFps = true;
	RenderCameraPosition = true;
	RenderDragInfo = true;
	RenderMouseInfo = true;
	RenderTrackdInfo = true;
	RenderStatusTextOnFinish = false;
	EnableRenderStatusText = false;

	if (GetUtility().IsTrackdEnabled())
	{
		RenderTrackdInfo = true;
	}

	// Use the DICOM TCP client for downloading DICOM files.
	mDicomClient.SetDownloadCallback(_handleDownload);
}

VimridViewer::~VimridViewer()
{
}

void VimridViewer::defaultCamera()
{
	// Good for workstation
	/*mCameraAnimationTarget.X = 30;
	mCameraAnimationTarget.Y = 40;
	mCameraAnimationTarget.Z = -12.15;*/

	// Good for theatre
	mCameraAnimationTarget.X = 40;
	mCameraAnimationTarget.Y = 15;
	mCameraAnimationTarget.Z = -14;

	mLastCameraDragVector = getCameraDragVector();
	mCameraAnimationBuffer = mCameraAnimationTarget;
}

VFloat VimridViewer::GetAspectRatio() const
{
	return this->VimridApplication::GetAspectRatio();
}

void VimridViewer::Init()
{
	this->GlutApplication::Init();

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Setup after everything else has initialised.
	setupInterface();

	// Tell the slices to stay out once scrolled out.
	GetMriVisualiser().GetScrollController().ChangeScrollMode(SM_LEAVE_OUT);

	/*for (int i = 0; i < 15; i++)
	{
		GetMriVisualiser().AddTextureAsSlice(new SampleTexture());
	}*/

	// Default camera after init so drag is based on trackd position.
	defaultCamera();

	/*// Reset cursor so it appears centered in menu.
	GetUtility().ResetCursor();

	// Show during init to ensure UI container has been setup.
	mMainMenu->Show();*/
}

VPoint3f VimridViewer::getCameraDragVector() const
{
	return GetDragController().GetMode(VIEW_DRAG_MODE).GetDragVector() * 0.2;
}

void VimridViewer::Update()
{
	this->GlutApplication::Update();

	// Calculate a general speed based on delta.
	VFloat deltaSpeed = GetUtility().GetDeltaTime() * 0.1;

	VPoint3f dragVector = getCameraDragVector();

	VFloat mDragSpeed = 1;
	mCameraDragDelta = dragVector - mLastCameraDragVector;
	mCameraAnimationTarget += mCameraDragDelta * mDragSpeed * deltaSpeed;

	VFloat maximumY = 170.0;
	if (mCameraAnimationTarget.Y > maximumY)
	{
		mCameraAnimationTarget.Y = maximumY;
	}

	VFloat minimumY = 5.0;
	if (mCameraAnimationTarget.Y < minimumY)
	{
		mCameraAnimationTarget.Y = minimumY;
	}

	VFloat minZ = -25.0;
	if (mCameraAnimationTarget.Z < minZ)
	{
		mCameraAnimationTarget.Z = minZ;
	}

	VFloat maxZ = -7.5;
	if (mCameraAnimationTarget.Z > maxZ)
	{
		mCameraAnimationTarget.Z = maxZ;
	}

	const VFloat zSpeed = 0.03;
	const VFloat fpSpeed = 0.03;
	const VFloat esSpeed = 0.01;

	if (IsSpecialKeyPressed(GLUT_KEY_UP))
	{
		mCameraAnimationTarget.Z += zSpeed * deltaSpeed;
	}

	if (IsSpecialKeyPressed(GLUT_KEY_DOWN))
	{
		mCameraAnimationTarget.Z -= zSpeed * deltaSpeed;
	}

	if (IsSpecialKeyPressed(GLUT_KEY_PAGE_UP))
	{
		GetFixationPoint() -= fpSpeed * deltaSpeed;
	}

	if (IsSpecialKeyPressed(GLUT_KEY_PAGE_DOWN))
	{
		GetFixationPoint() += fpSpeed * deltaSpeed;
	}

	if (IsSpecialKeyPressed(GLUT_KEY_LEFT))
	{
		GetEyeSeparation() -= esSpeed * deltaSpeed;
	}

	if (IsSpecialKeyPressed(GLUT_KEY_RIGHT))
	{
		GetEyeSeparation() += esSpeed * deltaSpeed;
	}

	UpdateSprites();
	GetMriVisualiser().Update();

	// Keep checking to see if there are textures that need creating.
	if (GetTextureManager().IsCreatePending())
	{
		GetTextureManager().CreateAll();
		WRITE_DEBUG_TEXT("Textures were created.");
	}

	string te = (GetUtility().IsTrackdEnabled() ? "Enabled" : "Disabled");
	string se = (IsStereoEnabled() ? "Enabled" : "Disabled");
	ADD_STATUS_LINE(
		"[Motion Sensing: " << te << "] " <<
		"[Stereography: " << se << "]");

	VFloat fp = GetFixationPoint();
	VFloat es = GetEyeSeparation();
	ADD_STATUS_LINE(
		"[Fixation Point: " << fp << "] " <<
		"[Eye Separation: " << es << "]");

	ADD_STATUS_LINE("[Smooth Cursor: " << GetUtility().GetCursorPositionSmooth() << "]");

	/* HACK: Big hack! This needs making more robust, currently it just
	 * looks at the Y axis and the code is very difficult to modify. Should
	 * also be abstracted out of this class. */
	/*VFloat minimumY = 5.0;
	VFloat maximumY = 170.0;
	VPoint3f baseCamera = this->GlutApplication::GetCameraVector();

	if (GetCameraVector().Y < minimumY)
	{
		// Only counter if the cursor was moved down.
		if ((mLastCameraVector.Y - GetCameraVector().Y) > 0)
		{
			// Record the amount that the user has over-stepped.
			mCameraCouteract.Y = baseCamera.Y - minimumY;
		}
	}

	if (GetCameraVector().Y > maximumY)
	{
		// Counter if moved upward.
		if ((mLastCameraVector.Y - GetCameraVector().Y) < 0)
		{
			mCameraCouteract.Y = baseCamera.Y - maximumY;
		}
	}

	ADD_STATUS_LINE("[Camera Counteract: " << mCameraCouteract << "]");
	mLastCameraVector = GetCameraVector();*/

	/* Smoothly move the camera toward the target based on delta time so
	 * it's constant over all computers. Don't bother altering if the sync
	 * is only off by 0.1 point (otherwise camera twich occurs).
	 */
	VPoint3f animateDistance = mCameraAnimationTarget - mCameraAnimationBuffer;
	//if ((trimSync > VPoint3f(0.1, 0.1, 0.1)) || (trimSync < VPoint3f(-0.1, -0.1, -0.1)))
	{
		const VFloat animateSpeed = 0.05;
		mCameraAnimationBuffer += animateDistance * animateSpeed * deltaSpeed;
	}

	mLastCameraDragVector = dragVector;

	if (!mMenuAutoShown)
	{
		// Reset cursor so it appears centered in menu.
		GetUtility().ResetCursor();

		// Show during init to ensure UI container has been setup.
		mMainMenu->Show();

		mMenuAutoShown = true;
	}
}

VPoint3f VimridViewer::GetCameraVector()
{
	/*VPoint3f vector = this->GlutApplication::GetCameraVector();
	return vector - mCameraCouteract + mCameraAnimationBuffer;*/
	return mCameraAnimationBuffer;
}

void VimridViewer::Render()
{
	this->GlutApplication::Render();

	// Use dark gray which is good for stereo.
	glClearColor(0.2, 0.2, 0.2, 1.0);

	if (IsStereoEnabled())
	{
		RenderStereoBuffer(GL_BACK_LEFT);
		RenderStereoBuffer(GL_BACK_RIGHT);
	}
	else
	{
		glDrawBuffer(GL_BACK);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		// Set a normal frustum for non-stereo viewers.
		glFrustum(
			-1.0, 1.0,
			-GetAspectRatio(),
			GetAspectRatio(),
			GetFrustumNearClip(),
			GetFrustumFarClip());

		RenderDelegate();
	}

	FinishRender();
}

void VimridViewer::RenderDelegate()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glTranslatef(0, 0, GetCameraVector().Z);
	glRotatef(GetCameraVector().Y, 1, 0, 0);
	glRotatef(GetCameraVector().X, 0, 1, 0);

	// Make sprites transparent if neccecary.
	VFloat alpha = mAlphaEnabled ? 0.6 : 1.0;
	glColor4f(1, 1, 1, alpha);

	RenderSprites();
	RenderStatusText();
}

void VimridViewer::Reshape()
{
	this->GlutApplication::Reshape();
	glViewport(0, 0, (GLsizei)GetWindowSize().Width, (GLsizei)GetWindowSize().Height);
}

void VimridViewer::setupInterface()
{
	const VColour4F buttonBackground = VColour4F(0.85, 0.85, 0.87, 0.7);
	const VColour4F buttonTextColour = VColour4F(0.2, 0.2, 0.22, 1.0);
	const VColour4F labelTextColour = VColour4F(0.8, 0.8, 0.8, 1.0);

	mMainMenu = new VimridMenu(*this);
	VimridMenu &mainMenu = *mMainMenu;
	mainMenu.SetName("mainMenu");
	mainMenu.SetSize(VSizeF(230,380));
	mainMenu.AddToggleMode(UI_MTM_HIDE);
	mainMenu.AddToggleMode(UI_MTM_SHOW);
	GetUiContainer().AddChild(mainMenu);

	if (GetUtility().IsTrackdEnabled())
	{
		/* When trackd is enabled, centre the cursor in the middle of the
		 * menu so the user doesn't need to move it to the menu area manually. */
		mainMenu.AddToggleMode(UI_MTM_CENTRE_CURSOR);
	}

	VSizeF buttonSize = VSizeF(mainMenu.GetSize().Width - 10, 40);

	VimridMenu &dicomMenu = *new VimridMenu(*this);
	dicomMenu.SetName("dicomMenu");
	dicomMenu.SetSize(VSizeF(300,230));
	dicomMenu.AddToggleMode(UI_MTM_HIDE);
	GetUiContainer().AddChild(dicomMenu);

	VimridMenu &filterMenu = *new VimridMenu(*this);
	filterMenu.SetName("filtersMenu");
	filterMenu.SetSize(VSizeF(300,280));
	filterMenu.AddToggleMode(UI_MTM_HIDE);
	GetUiContainer().AddChild(filterMenu);

	mDicomLoadingScreen = new VimridMenu(*this);
	VimridMenu &dicomLoadingScreen = *mDicomLoadingScreen;
	dicomLoadingScreen.SetName("dicomLoadingScreen");
	dicomLoadingScreen.SetSize(VSizeF(210,95));
	dicomLoadingScreen.AddToggleMode(UI_MTM_SUSPEND);
	GetUiContainer().AddChild(dicomLoadingScreen);

	mFilterLoadingScreen = new VimridMenu(*this);
	VimridMenu &filterLoadingScreen = *mFilterLoadingScreen;
	filterLoadingScreen.SetName("filterLoadingScreen");
	filterLoadingScreen.SetSize(VSizeF(210,95));
	filterLoadingScreen.AddToggleMode(UI_MTM_SUSPEND);
	GetUiContainer().AddChild(filterLoadingScreen);

	Label &mainTitleLabel = *new Label("ViMRID - Main Menu");
	mainTitleLabel.SetPosition(VPoint2f(5, 7));
	mainTitleLabel.SetTextColour(labelTextColour);
	mainMenu.AddChild(mainTitleLabel);

	Button &loadDicomButton = *new Button("Load DICOM Data");
	loadDicomButton.SetName("loadDicomButton");
	loadDicomButton.SetPosition(VPoint2f(5, 35));
	loadDicomButton.SetSize(buttonSize);
	loadDicomButton.SetBackground(buttonBackground);
	loadDicomButton.SetTextColour(buttonTextColour);
	loadDicomButton.SetShowControl(dicomMenu);
	loadDicomButton.AddSelectAction(UI_BSA_HIDE_PARENT);
	loadDicomButton.AddSelectAction(UI_BSA_SHOW_CONTROL);
	mainMenu.AddChild(loadDicomButton);

	mFilterMenuButton = new Button("Apply Image Filters");
	Button &filterMenuButton = *mFilterMenuButton;
	filterMenuButton.SetName("filterMenuButton");
	filterMenuButton.SetPosition(VPoint2f(5, 85));
	filterMenuButton.SetSize(buttonSize);
	filterMenuButton.SetBackground(buttonBackground);
	filterMenuButton.SetTextColour(buttonTextColour);
	filterMenuButton.SetShowControl(filterMenu);
	filterMenuButton.AddSelectAction(UI_BSA_HIDE_PARENT);
	filterMenuButton.AddSelectAction(UI_BSA_SHOW_CONTROL);
	filterMenuButton.Disable();
	mainMenu.AddChild(filterMenuButton);

	Button &toggleStatusButton = *new Button("Toggle Status Text");
	toggleStatusButton.SetName("toggleStatusButton");
	toggleStatusButton.SetPosition(VPoint2f(5, 135));
	toggleStatusButton.SetSize(buttonSize);
	toggleStatusButton.SetBackground(buttonBackground);
	toggleStatusButton.SetTextColour(buttonTextColour);
	mainMenu.AddChild(toggleStatusButton);

	mToggleStereoButton = new Button("Toggle Stereography");
	Button &toggleStereoButton = *mToggleStereoButton;
	toggleStereoButton.SetName("toggleStereoButton");
	toggleStereoButton.SetPosition(VPoint2f(5, 185));
	toggleStereoButton.SetSize(buttonSize);
	toggleStereoButton.SetBackground(buttonBackground);
	toggleStereoButton.SetTextColour(buttonTextColour);
	mainMenu.AddChild(toggleStereoButton);

	mToggleTrackdButton = new Button("Toggle Motion Sensing");
	Button &toggleTrackdButton = *mToggleTrackdButton;
	toggleTrackdButton.SetName("toggleTrackdButton");
	toggleTrackdButton.SetPosition(VPoint2f(5, 235));
	toggleTrackdButton.SetSize(buttonSize);
	toggleTrackdButton.SetBackground(buttonBackground);
	toggleTrackdButton.SetTextColour(buttonTextColour);
	mainMenu.AddChild(toggleTrackdButton);

	// Disable motion sensor toggle if disabled.
	if (!GetUtility().IsTrackdSupported())
	{
		toggleTrackdButton.Disable();
	}

	mToggleAlphaButton = new Button("Toggle Transparency");
	Button &toggleAlphaButton = *mToggleAlphaButton;
	toggleAlphaButton.SetName("toggleAlphaButton");
	toggleAlphaButton.SetPosition(VPoint2f(5, 285));
	toggleAlphaButton.SetSize(buttonSize);
	toggleAlphaButton.SetBackground(buttonBackground);
	toggleAlphaButton.SetTextColour(buttonTextColour);
	mainMenu.AddChild(toggleAlphaButton);

	mExitVimridButton = new Button("Exit ViMRID");
	Button &exitVimridButton = *mExitVimridButton;
	exitVimridButton.SetName("exitVimridButton");
	exitVimridButton.SetPosition(VPoint2f(5, 335));
	exitVimridButton.SetSize(buttonSize);
	exitVimridButton.SetBackground(buttonBackground);
	exitVimridButton.SetTextColour(buttonTextColour);
	mainMenu.AddChild(exitVimridButton);

	Label &dicomTitleLabel = *new Label("ViMRID - Load DICOM Data");
	dicomTitleLabel.SetPosition(VPoint2f(5, 7));
	dicomTitleLabel.SetTextColour(labelTextColour);
	dicomMenu.AddChild(dicomTitleLabel);

	Button &loadDicomSet1Button = *new Button("SDY00000/SRS00000");
	loadDicomSet1Button.SetName("loadDicomSet1Button");
	loadDicomSet1Button.SetPosition(VPoint2f(5, 35));
	loadDicomSet1Button.SetSize(VSizeF(dicomMenu.GetSize().Width - 10, 40));
	loadDicomSet1Button.SetBackground(buttonBackground);
	loadDicomSet1Button.SetTextColour(buttonTextColour);
	loadDicomSet1Button.SetShowControl(dicomLoadingScreen);
	loadDicomSet1Button.AddSelectAction(UI_BSA_HIDE_PARENT);
	loadDicomSet1Button.AddSelectAction(UI_BSA_SHOW_CONTROL);
	dicomMenu.AddChild(loadDicomSet1Button);

	Button &loadDicomSet2Button = *new Button("SDY00000/SRS00001");
	loadDicomSet2Button.SetName("loadDicomSet2Button");
	loadDicomSet2Button.SetPosition(VPoint2f(5, 85));
	loadDicomSet2Button.SetSize(VSizeF(dicomMenu.GetSize().Width - 10, 40));
	loadDicomSet2Button.SetBackground(buttonBackground);
	loadDicomSet2Button.SetTextColour(buttonTextColour);
	loadDicomSet2Button.SetShowControl(dicomLoadingScreen);
	loadDicomSet2Button.AddSelectAction(UI_BSA_HIDE_PARENT);
	loadDicomSet2Button.AddSelectAction(UI_BSA_SHOW_CONTROL);
	dicomMenu.AddChild(loadDicomSet2Button);

	Button &loadDicomSet3Button = *new Button("SDY00001/SRS00000");
	loadDicomSet3Button.SetName("loadDicomSet3Button");
	loadDicomSet3Button.SetPosition(VPoint2f(5, 135));
	loadDicomSet3Button.SetSize(VSizeF(dicomMenu.GetSize().Width - 10, 40));
	loadDicomSet3Button.SetBackground(buttonBackground);
	loadDicomSet3Button.SetTextColour(buttonTextColour);
	loadDicomSet3Button.SetShowControl(dicomLoadingScreen);
	loadDicomSet3Button.AddSelectAction(UI_BSA_HIDE_PARENT);
	loadDicomSet3Button.AddSelectAction(UI_BSA_SHOW_CONTROL);
	dicomMenu.AddChild(loadDicomSet3Button);

	Button &loadDicomSet4Button = *new Button("SDY00001/SRS00001");
	loadDicomSet4Button.SetName("loadDicomSet4Button");
	loadDicomSet4Button.SetPosition(VPoint2f(5, 185));
	loadDicomSet4Button.SetSize(VSizeF(dicomMenu.GetSize().Width - 10, 40));
	loadDicomSet4Button.SetBackground(buttonBackground);
	loadDicomSet4Button.SetTextColour(buttonTextColour);
	loadDicomSet4Button.SetShowControl(dicomLoadingScreen);
	loadDicomSet4Button.AddSelectAction(UI_BSA_HIDE_PARENT);
	loadDicomSet4Button.AddSelectAction(UI_BSA_SHOW_CONTROL);
	dicomMenu.AddChild(loadDicomSet4Button);

	Label &dicomLoadingScreenLabel = *new Label("Loading DICOM data...");
	dicomLoadingScreenLabel.SetPosition(VPoint2f(10, 10));
	dicomLoadingScreenLabel.SetTextColour(labelTextColour);
	dicomLoadingScreen.AddChild(dicomLoadingScreenLabel);

	Button &dicomLoadingScreenCancelButton = *new Button("Cancel");
	dicomLoadingScreenCancelButton.SetName("dicomLoadingScreenCancelButton");
	dicomLoadingScreenCancelButton.SetPosition(VPoint2f(65, 40));
	dicomLoadingScreenCancelButton.SetSize(VSizeF(75, 40));
	dicomLoadingScreenCancelButton.SetBackground(buttonBackground);
	dicomLoadingScreenCancelButton.SetTextColour(buttonTextColour);
	dicomLoadingScreenCancelButton.SetShowControl(dicomMenu);
	dicomLoadingScreenCancelButton.AddSelectAction(UI_BSA_HIDE_PARENT);
	dicomLoadingScreenCancelButton.AddSelectAction(UI_BSA_SHOW_CONTROL);
	dicomLoadingScreen.AddChild(dicomLoadingScreenCancelButton);

	Label &filterMenuTitleLabel = *new Label("ViMRID - Apply Image Filters");
	filterMenuTitleLabel.SetPosition(VPoint2f(5, 7));
	filterMenuTitleLabel.SetTextColour(labelTextColour);
	filterMenu.AddChild(filterMenuTitleLabel);

	mSobelXNormalButton = new Button("Sobel X (Normal)");
	Button &sobelXNormalButton = *mSobelXNormalButton;
	sobelXNormalButton.SetName("sobelXNormalButton");
	sobelXNormalButton.SetPosition(VPoint2f(5, 35));
	sobelXNormalButton.SetSize(VSizeF(filterMenu.GetSize().Width - 10, 40));
	sobelXNormalButton.SetBackground(buttonBackground);
	sobelXNormalButton.SetTextColour(buttonTextColour);
	sobelXNormalButton.SetShowControl(filterLoadingScreen);
	sobelXNormalButton.AddSelectAction(UI_BSA_HIDE_PARENT);
	sobelXNormalButton.AddSelectAction(UI_BSA_SHOW_CONTROL);
	filterMenu.AddChild(sobelXNormalButton);

	mSobelYNormalButton = new Button("Sobel Y (Normal)");
	Button &sobelYNormalButton = *mSobelYNormalButton;
	sobelYNormalButton.SetName("sobelYNormalButton");
	sobelYNormalButton.SetPosition(VPoint2f(5, 85));
	sobelYNormalButton.SetSize(VSizeF(filterMenu.GetSize().Width - 10, 40));
	sobelYNormalButton.SetBackground(buttonBackground);
	sobelYNormalButton.SetTextColour(buttonTextColour);
	sobelYNormalButton.SetShowControl(filterLoadingScreen);
	sobelYNormalButton.AddSelectAction(UI_BSA_HIDE_PARENT);
	sobelYNormalButton.AddSelectAction(UI_BSA_SHOW_CONTROL);
	filterMenu.AddChild(sobelYNormalButton);

	mSobelXYNormalButton = new Button("Sobel X+Y (Normal)");
	Button &sobelXYNormalButton = *mSobelXYNormalButton;
	sobelXYNormalButton.SetName("sobelXYNormalButton");
	sobelXYNormalButton.SetPosition(VPoint2f(5, 135));
	sobelXYNormalButton.SetSize(VSizeF(filterMenu.GetSize().Width - 10, 40));
	sobelXYNormalButton.SetBackground(buttonBackground);
	sobelXYNormalButton.SetTextColour(buttonTextColour);
	sobelXYNormalButton.SetShowControl(filterLoadingScreen);
	sobelXYNormalButton.AddSelectAction(UI_BSA_HIDE_PARENT);
	sobelXYNormalButton.AddSelectAction(UI_BSA_SHOW_CONTROL);
	filterMenu.AddChild(sobelXYNormalButton);

	mSobelXYScopedButton = new Button("Sobel X+Y (Scoped)");
	Button &sobelXYScopedButton = *mSobelXYScopedButton;
	sobelXYScopedButton.SetName("sobelXYScopedButton");
	sobelXYScopedButton.SetPosition(VPoint2f(5, 185));
	sobelXYScopedButton.SetSize(VSizeF(filterMenu.GetSize().Width - 10, 40));
	sobelXYScopedButton.SetBackground(buttonBackground);
	sobelXYScopedButton.SetTextColour(buttonTextColour);
	sobelXYScopedButton.SetShowControl(filterLoadingScreen);
	sobelXYScopedButton.AddSelectAction(UI_BSA_HIDE_PARENT);
	sobelXYScopedButton.AddSelectAction(UI_BSA_SHOW_CONTROL);
	filterMenu.AddChild(sobelXYScopedButton);

	mFilterRestoreButton = new Button("Restore source image");
	Button &filterRestoreButton = *mFilterRestoreButton;
	filterRestoreButton.SetName("filterRestoreButton");
	filterRestoreButton.SetPosition(VPoint2f(5, 235));
	filterRestoreButton.SetSize(VSizeF(filterMenu.GetSize().Width - 10, 40));
	filterRestoreButton.SetBackground(buttonBackground);
	filterRestoreButton.SetTextColour(buttonTextColour);
	filterMenu.AddChild(filterRestoreButton);

	Label &filterLoadingScreenLabel = *new Label("Applying image filter...");
	filterLoadingScreenLabel.SetPosition(VPoint2f(10, 10));
	filterLoadingScreenLabel.SetTextColour(labelTextColour);
	filterLoadingScreen.AddChild(filterLoadingScreenLabel);

	mFilterLoadingScreenCancelButton = new Button("Cancel");
	Button &filterLoadingScreenCancelButton = *mFilterLoadingScreenCancelButton;
	filterLoadingScreenCancelButton.SetName("filterLoadingScreenCancelButton");
	filterLoadingScreenCancelButton.SetPosition(VPoint2f(65, 40));
	filterLoadingScreenCancelButton.SetSize(VSizeF(75, 40));
	filterLoadingScreenCancelButton.SetBackground(buttonBackground);
	filterLoadingScreenCancelButton.SetTextColour(buttonTextColour);
	filterLoadingScreenCancelButton.SetShowControl(filterMenu);
	filterLoadingScreenCancelButton.AddSelectAction(UI_BSA_HIDE_PARENT);
	filterLoadingScreenCancelButton.AddSelectAction(UI_BSA_SHOW_CONTROL);
	filterLoadingScreen.AddChild(filterLoadingScreenCancelButton);
}

void VimridViewer::OnControlSelectRelease(Control &control)
{
	// HACK: Comparing text when pointer should be compared.
	if ((control.GetName() == "loadDicomSet1Button") ||
		(control.GetName() == "loadDicomSet2Button") ||
		(control.GetName() == "loadDicomSet3Button") ||
		(control.GetName() == "loadDicomSet4Button"))
	{
		const Button *buttonPtr = dynamic_cast<const Button*>(&control);
		mDicomClient.DownloadAsync(buttonPtr->GetText());
	}

	// HACK: Comparing text when pointer should be compared.
	if (control.GetName() == "toggleStatusButton")
	{
		EnableRenderStatusText = !EnableRenderStatusText;
	}

	// Cancel loading of DICOM data.
	if (&control == mFilterLoadingScreenCancelButton)
	{
		mDicomLoadCancel = true;
	}

	// Turns transparency on and off.
	if (&control == mToggleAlphaButton)
	{
		mAlphaEnabled = !mAlphaEnabled;
	}

	if (&control == mFilterRestoreButton)
	{
		pthread_create(&mFilterRestoreThread, NULL, _filterRestore, NULL);
	}

	/* HACK: If any other button pressed, filter mode is reset. This
	 * conveniently does the same thing as what the cancel button does
	 * so it works fine for the time being, but this should only really
	 * be reset on specific scenarios (such as cancel button press).
	 */
	mFilterMode = VV_FM_None;
	if (&control == mSobelXNormalButton)
	{
		mFilterMode = VV_FM_SobelXNormal;
	}
	else if (&control == mSobelYNormalButton)
	{
		mFilterMode = VV_FM_SobelYNormal;
	}
	else if (&control == mSobelXYNormalButton)
	{
		mFilterMode = VV_FM_SobelXYNormal;
	}
	else if (&control == mSobelXYScopedButton)
	{
		mFilterMode = VV_FM_SobelXYScoped;
	}

	// Once filter mode potentially set, check and if it is, process!
	if (mFilterMode != VV_FM_None)
	{
		pthread_create(&mProcessorThread, NULL, _processImages, NULL);
	}

	if (&control == mToggleStereoButton)
	{
		if (IsStereoEnabled())
		{
			DisableStereo();
		}
		else
		{
			EnableStereo();
		}
	}

	if (&control == mToggleTrackdButton)
	{
		VimridMenu &mainMenu = *mMainMenu;
		if (GetUtility().IsTrackdEnabled())
		{
			GetUtility().DisableTrackd();
			GetUiContainer().ResetCursor();
			if (mainMenu.HasToggleMode(UI_MTM_CENTRE_CURSOR))
			{
				mainMenu.RemoveToggleMode(UI_MTM_CENTRE_CURSOR);
			}
		}
		else
		{
			GetUtility().EnableTrackd();
			GetUiContainer().CentreCursor(*mMainMenu);
			if (!mainMenu.HasToggleMode(UI_MTM_CENTRE_CURSOR))
			{
				mainMenu.AddToggleMode(UI_MTM_CENTRE_CURSOR);
			}
		}
	}

	if (&control == mExitVimridButton)
	{
		Exit();
	}
}

void VimridViewer::OnControlShown(Control &control)
{
	// If a menu was shown, and feature isn't already pushed back, push back.
	if ((dynamic_cast<VimridMenu*>(&control) != NULL)
			&& GetUiContainer().HasVisibleChild()
			&& !mCameraPushedBack)
	{
		const VFloat paralaxTarget = -16;
		mCameraPushbackSaved = mCameraAnimationTarget;

		//DEBUG("CameraTrim was: " << CameraTrim);
		//DEBUG("GetCameraVector() was: " << GetCameraVector());

		//CameraTrim.Z += (positiveParalax - GetCameraVector().Z);

		mCameraAnimationTarget.Z += (paralaxTarget - mCameraAnimationTarget.Z);

		// Just for fun, rotate on x and y as well to make it look cool.
		const VFloat rollX = -20;
		const VFloat rollY = 20;

		mCameraAnimationTarget.X += rollX;
		mCameraAnimationTarget.Y += rollY;

		//DEBUG("CameraTrim now: " << CameraTrim);
		//DEBUG("GetCameraVector() now: " << GetCameraVector());

		mCameraPushedBack = true;
	}

	this->GlutApplication::OnControlShown(control);
}

void VimridViewer::OnControlHidden(Control &control)
{
	// If a menu was hidden and feature is pushed back, restore.
	if ((dynamic_cast<VimridMenu*>(&control) != NULL)
			&& !GetUiContainer().HasVisibleChild()
			&& mCameraPushedBack)
	{
		mCameraAnimationTarget = mCameraPushbackSaved;
		//CameraTrim = mCameraTrimSaved;
		//DEBUG("CameraTrim revert: " << CameraTrim);

		mCameraPushedBack = false;
	}

	this->GlutApplication::OnControlHidden(control);
}

void VimridViewer::processImages()
{
	// First remove any existing images.
	GetMriVisualiser().Clear();

	vector<DicomImage*> images = mDicomResult->GetImages();
	for (VUInt32 i = 0; i < images.size(); i++)
	{
		DicomImage &sourceDicom = *images[i];
		ImageMatrix &sourceImage = *sourceDicom.ToImageMatrix();

		// All use sobel so just create a single processor.
		ImageProcessor processor(sourceImage, SobelOperator());

		switch(mFilterMode)
		{
			case VV_FM_SobelXNormal:
			{
				processor.FilterRotations = 1;
			}
			break;
			case VV_FM_SobelYNormal:
			{
				processor.FilterRotations = 1;
				processor.StartAngle = 90;
			}
			break;
			case VV_FM_SobelXYNormal:
			{
				processor.FilterRotations = 2;
			}
			break;
			case VV_FM_SobelXYScoped:
			{
				processor.FilterRotations = 2;
				processor.ScopeMin = 0.5;
				processor.ScopeMax = 0.8;
			}
			break;
			case VV_FM_None:
			{
				// HACK: Exit on cancel should be more elegant.
				GetMriVisualiser().Clear();
				return;
			}
			break;
		}

		WRITE_DEBUG_TEXT("Processing default Sobel...");
		ImageMatrix &sobelDefaultImage = *processor.ProcessImage();
		GetMriVisualiser().AddImageAsSlice(sobelDefaultImage);

		// Delete image as it's not used by anything.
		delete &sourceImage;
	}

	// Once finished hide loading notice.
	mFilterLoadingScreen->Hide();
}

void *VimridViewer::_processImages(void *argPtr)
{
	((VimridViewer*)VimridApplication::GetInstance())->processImages();
	return NULL;
}

// Static method to pass request back to the singleton.
void *VimridViewer::_handleDownload(void *argPtr)
{
	((VimridViewer*)VimridApplication::GetInstance())->handleDownload(argPtr);
	return NULL;
}

void VimridViewer::handleDownload(void *argPtr)
{
	if (mDicomResult != NULL)
	{
		// Delete the previous data.
		delete mDicomResult;
	}

	// Only load images if download not cancelled.
	if (!mDicomLoadCancel)
	{
		mDicomResult = mDicomClient.GetResult(argPtr);
		if (mDicomResult->IsSuccessful())
		{
			GetMriVisualiser().Clear();
			GetMriVisualiser().AddDicomImages(mDicomResult->GetImages());

			mDicomLoadingScreen->Hide();
			mFilterMenuButton->Enable();
		}
	}
	else
	{
		mDicomLoadCancel = false;
	}
}

void *VimridViewer::_filterRestore(void *argPtr)
{
	((VimridViewer*)VimridApplication::GetInstance())->filterRestore();
	return NULL;
}

void VimridViewer::filterRestore()
{
	GetMriVisualiser().Clear();
	GetMriVisualiser().AddDicomImages(mDicomResult->GetImages());
}

}
}
