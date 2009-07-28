/* Shows how a stationary menu can be rendered transparently in front of
 * an object in the background.
 *
 * This is the first experiment building toward the final HCI. However, the
 * coloured cube is not being rendered correctly when GL_BLEND is enabled;
 * this could be because the vertices have not been coloured correctly.
 *
 *  Created: 02-Apr-2009
 *   Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "InterfaceDemo2.h"
#include "../../mri/MriSet.h"
#include "../../mri/MriSlice.h"
#include "../../input/DragMode.h"
#include "../../textures/SampleTexture.h"
#include "../../ui/Label.h"
#include "../../ui/Slider.h"
#include "../../ui/Button.h"
#include "../../ui/Control.h"

using namespace std;
using namespace vimrid::input;
using namespace vimrid::textures;
using namespace vimrid::glut;
using namespace vimrid::mri;
using namespace vimrid::ui;
using namespace vimrid::dicom;

#define VIEW_DRAG_MODE 0 // LMB

#define DICOM_PORT 1234
//#define DICOM_HOST "home-ws-2-vm-1"
#define DICOM_HOST "localhost"
//#define DICOM_HOST "njbtmp"

namespace vimrid
{
namespace demos
{
namespace interface
{

InterfaceDemo2::InterfaceDemo2(
	VimridSettings &settings)
	:
	GlutApplication(settings),
	mDicomClient(DICOM_HOST, DICOM_PORT),
	mDicomResult(NULL),
	mDicomLoadingScreen(NULL)
{
	Title = "Interface demo 2 - Thu 30 Apr";
	EnableCursor = false;
	RenderFps = true;
	RenderCameraPosition = true;
	RenderDragInfo = true;
	RenderMouseInfo = true;
	CameraTrim.X = 30;
	CameraTrim.Y = 40;
	CameraTrim.Z = -4;

	setupMenu();

	// Use the DICOM TCP client for downloading DICOM files.
	mDicomClient.SetDownloadCallback(_handleDownload);
}

InterfaceDemo2::~InterfaceDemo2()
{
}

void InterfaceDemo2::Init()
{
	this->GlutApplication::Init();

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (int i = 0; i < 15; i++)
	{
		GetMriVisualiser().AddTextureAsSlice(new SampleTexture());
	}
}

void InterfaceDemo2::Update()
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

void InterfaceDemo2::Render()
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

void InterfaceDemo2::Reshape()
{
	this->GlutApplication::Reshape();

	glViewport(0, 0, (GLsizei)GetWindowSize().Width, (GLsizei)GetWindowSize().Height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, (GLfloat)GetWindowSize().Width / (GLfloat)GetWindowSize().Height, 1.0, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void InterfaceDemo2::setupMenu()
{
	const VColour4F buttonBackground = VColour4F(0.85, 0.85, 0.87, 0.7);
	const VColour4F buttonTextColour = VColour4F(0.2, 0.2, 0.22, 1.0);
	const VColour4F labelTextColour = VColour4F(0.8, 0.8, 0.8, 1.0);

	VimridMenu &mainMenu = *new VimridMenu(*this);
	mainMenu.SetName("mainMenu");
	mainMenu.SetSize(VSizeF(200,300));
	mainMenu.AddToggleMode(UI_MTM_HIDE);
	mainMenu.AddToggleMode(UI_MTM_SHOW);
	GetUiContainer().AddChild(mainMenu);

	VimridMenu &dicomMenu = *new VimridMenu(*this);
	dicomMenu.SetName("dicomMenu");
	dicomMenu.SetSize(VSizeF(300,300));
	dicomMenu.AddToggleMode(UI_MTM_HIDE);
	GetUiContainer().AddChild(dicomMenu);

	VimridMenu &dicomLoadingScreen = *new VimridMenu(*this);
	dicomLoadingScreen.SetName("dicomLoadingScreen");
	dicomLoadingScreen.SetSize(VSizeF(195,95));
	dicomLoadingScreen.AddToggleMode(UI_MTM_SUSPEND);
	GetUiContainer().AddChild(dicomLoadingScreen);
	mDicomLoadingScreen = &dicomLoadingScreen;

	Label &mainTitleLabel = *new Label("ViMRID - Main Menu");
	mainTitleLabel.SetPosition(VPoint2f(5, 5));
	mainTitleLabel.SetTextColour(labelTextColour);
	mainMenu.AddChild(mainTitleLabel);

	Button &loadDicomButton = *new Button("Load DICOM Data");
	loadDicomButton.SetName("loadDicomButton");
	loadDicomButton.SetPosition(VPoint2f(5, 35));
	loadDicomButton.SetSize(VSizeF(mainMenu.GetSize().Width - 10, 40));
	loadDicomButton.SetBackground(buttonBackground);
	loadDicomButton.SetTextColour(buttonTextColour);
	loadDicomButton.SetShowControl(dicomMenu);
	loadDicomButton.AddSelectAction(UI_BSA_HIDE_PARENT);
	loadDicomButton.AddSelectAction(UI_BSA_SHOW_CONTROL);
	mainMenu.AddChild(loadDicomButton);

	Button &applyFiltersButton = *new Button("Apply Image Filter");
	applyFiltersButton.SetPosition(VPoint2f(5, 85));
	applyFiltersButton.SetSize(VSizeF(mainMenu.GetSize().Width - 10, 40));
	applyFiltersButton.SetBackground(buttonBackground);
	applyFiltersButton.SetTextColour(buttonTextColour);
	mainMenu.AddChild(applyFiltersButton);

	Slider &motionSensingSlider = *new Slider("Motion Sensing");
	motionSensingSlider.SetPosition(VPoint2f(5, 120));
	mainMenu.AddChild(motionSensingSlider);

	Label &dicomTitleLabel = *new Label("ViMRID - Load DICOM Data");
	dicomTitleLabel.SetPosition(VPoint2f(5, 5));
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

	Label &dicomLoadingScreenLabel = *new Label("Please wait, loading...");
	dicomLoadingScreenLabel.SetPosition(VPoint2f(10, 10));
	dicomLoadingScreenLabel.SetTextColour(labelTextColour);
	dicomLoadingScreen.AddChild(dicomLoadingScreenLabel);

	Button &dicomLoadingScreenCancelButton = *new Button("Cancel");
	dicomLoadingScreenCancelButton.SetName("dicomLoadingScreenCancelButton");
	dicomLoadingScreenCancelButton.SetPosition(VPoint2f(62, 40));
	dicomLoadingScreenCancelButton.SetSize(VSizeF(75, 40));
	dicomLoadingScreenCancelButton.SetBackground(buttonBackground);
	dicomLoadingScreenCancelButton.SetTextColour(buttonTextColour);
	dicomLoadingScreenCancelButton.SetShowControl(dicomMenu);
	dicomLoadingScreenCancelButton.AddSelectAction(UI_BSA_HIDE_PARENT);
	dicomLoadingScreenCancelButton.AddSelectAction(UI_BSA_SHOW_CONTROL);
	dicomLoadingScreen.AddChild(dicomLoadingScreenCancelButton);
}

void InterfaceDemo2::OnControlSelectRelease(vimrid::ui::Control &control)
{
	// HACK: Just doing this in a hacky way for the time being.
	if ((control.GetName() == "loadDicomSet1Button") ||
		(control.GetName() == "loadDicomSet2Button") ||
		(control.GetName() == "loadDicomSet3Button") ||
		(control.GetName() == "loadDicomSet4Button"))
	{
		const Button *buttonPtr = dynamic_cast<const Button*>(&control);
		mDicomClient.DownloadAsync(buttonPtr->GetText());
	}
}

// Static method to pass request back to the singleton.
void *InterfaceDemo2::_handleDownload(void *argPtr)
{
	((InterfaceDemo2*)VimridApplication::GetInstance())->handleDownload(argPtr);
	return NULL;
}

void InterfaceDemo2::handleDownload(void *argPtr)
{
	mDicomResult = mDicomClient.GetResult(argPtr);
	if (mDicomResult->IsSuccessful())
	{
		GetMriVisualiser().Clear();
		GetMriVisualiser().AddDicomImages(mDicomResult->GetImages());

		if (mDicomLoadingScreen != NULL)
		{
			mDicomLoadingScreen->Hide();
		}
	}
}

}
}
}
