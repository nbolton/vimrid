/* This demo focuses on implementing the cube from demos 1-4, to replace
 * the gears from glxgears. Also I will work on the wand drag feature.
 *
 * Tested on Reyer, seems to be some issues with the frustum is not following
 * the camera eye.
 */

#include "CubeDemo9.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;
using namespace vimrid::input;

namespace vimrid
{
namespace demos
{
namespace cubedemos
{

CubeDemo9::CubeDemo9(VimridSettings &settings)
:
	GlutApplication(settings),
	StereoApplication(5.0, 60.0, 45.0, 5.5)
{
	Title = "Cube demo 9 - 07/05/09";
	RenderFps = true;
	RenderCameraPosition = true;
	RenderStatusTextOnFinish = false;
	SetWindowSize(800, 800);

	wandDragActive = false;
	enableWandDrag = false;

	if (GetInputManager()->IsTrackdSupported())
	{
		GetInputManager()->InitTrackd(TD_TRACKER_KEY);
		glasses = &GetInputManager()->AddTrackdDevice(TD_GLASSES_ID);
		wand = &GetInputManager()->AddTrackdDevice(TD_WAND_ID, TD_WAND_CONTROLLER_KEY);
	}

	mouse = &GetInputManager()->GetMouseInfo();
}

VFloat CubeDemo9::GetAspectRatio() const
{
	return this->VimridApplication::GetAspectRatio();
}

void CubeDemo9::Init()
{
	this->GlutApplication::Init();

	glEnable(GL_CULL_FACE);

	createCube();
}

void CubeDemo9::Update()
{
	this->GlutApplication::Update();

	updateCamera();

	if (IsSpecialKeyPressed(GLUT_KEY_PAGE_UP))
	{
		GetEyeSeparation() += .01;
	}

	if (IsSpecialKeyPressed(GLUT_KEY_PAGE_DOWN))
	{
		// Don't allow eyes to cross the wrong way.
		if (GetEyeSeparation() > 0)
		{
			GetEyeSeparation() -= .01;
		}
	}
}

void CubeDemo9::Render()
{
	this->GlutApplication::Render();

	/* Set the background to a slightly gray/off-black colour
	 * so that the shadowing of the stereo effect is reduced,
	 * due to the colours not contrasting as badly as if it
	 * were solid black. White also looks bad as it exposes
	 * CRT flicker - but may look ok on a projector? */
	glClearColor(0.2, 0.2, 0.2, 1.0);

	if (GetSettings().EnableStereo)
	{
		RenderStereoBuffer(GL_BACK_LEFT);
		RenderStereoBuffer(GL_BACK_RIGHT);
	}
	else
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		// Set a normal frustum for non-stereo viewers.
		glFrustum(
			-1.0, 1.0,
			-GetAspectRatio(), GetAspectRatio(),
			GetFrustumNearClip(), GetFrustumFarClip());

		RenderDelegate();
	}

	FinishRender();
}

void CubeDemo9::RenderDelegate()
{
	glRotatef(CameraPoint.Y, 1, 0, 0);
	glRotatef(-CameraPoint.X, 0, 1, 0);

	// Rotate the cube depending on the wand motion sensor.
	glRotatef(FeatureRotation.X, 1, 0, 0);
	glRotatef(FeatureRotation.Y, 0, 1, 0);
	glRotatef(FeatureRotation.Z, 0, 0, 1);

	drawCube();

	RenderStatusText();
}

void CubeDemo9::Reshape()
{
	this->GlutApplication::Reshape();
	glViewport(0, 0, (int)GetWindowSize().Width, (int)GetWindowSize().Height);

	// Move camera away from object.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -40.0);
}

void CubeDemo9::updateCamera()
{
	if (IsSpecialKeyPressed(GLUT_KEY_UP))
	{
		GetFixationPoint() -= .01;
	}

	if (IsSpecialKeyPressed(GLUT_KEY_DOWN))
	{
		GetFixationPoint() += .01;
	}

	if (GetInputManager()->IsTrackdSupported())
	{
		// Move the camera to where the glasses are.
		CameraPoint.X = CameraTrim.X + (glasses->GetSensor().GetPosition().X) * 20;
		CameraPoint.Y = CameraTrim.Y + 100 + ((glasses->GetSensor().GetPosition().Z) * 30);
		CameraPoint.Z = CameraTrim.Z + 100 + (100 - ((glasses->GetSensor().GetPosition().Y) * 10));

		if (enableWandDrag)
		{
			// Allow the user to "grab" and rotate the object.
			if (wand->GetController().IsButtonPressed(TRACKD_BUTTON_1))
			{
				if (!wandDragActive)
				{
					wandDragActive = true;
					wandDragFrom.X = wand->GetSensor().GetPosition().X;
					wandDragFrom.Y = wand->GetSensor().GetPosition().Y;
					wandDragFrom.Z = wand->GetSensor().GetPosition().Z;
				}

				FeatureRotation.Z += (wand->GetSensor().GetPosition().X - wandDragFrom.X) * .001;
			}
			else
			{
				wandDragActive = false;
			}
		}
	}
	else
	{
		if (IsModiferKeyPressed(GLUT_ACTIVE_CTRL))
		{
			FeatureRotation.Z += (mouse->GetPosition().X - wandDragFrom.X) * .001;
		}
		else
		{
			// When mouse moves up and down, move toward/away from screen.
			CameraPoint.X = CameraTrim.X + (((GetWindowSize().Width / 2) * -1) + mouse->GetPosition().X) * 0.15;
			CameraPoint.Y = CameraTrim.Y + (((GetWindowSize().Height / 2) * -1) + mouse->GetPosition().Y) * -0.2;
			CameraPoint.Z = CameraTrim.Z + 50;// + (mouse->Point->Y) * 0.2;
		}
	}

}

void CubeDemo9::drawCube()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBegin(GL_QUADS);
	for (a = 0; a < 6; ++a)
	{
		for (b = 0; b < 4; ++b)
		{
			currentVer = cube.quad[a].ver[b];
			glColor3fv(cube.ver[currentVer].col);
			glVertex3fv(cube.ver[currentVer].pos);
		}
	}
	glEnd();
}

void CubeDemo9::createCube()
{
	// Plus symbols added to increase readability.
	// This defines the position of each vertex.
  float  vertexPosDat[8][3]=
  {
	{-3.5, +3.5, +3.5}, //left,top,front
	{+3.5, +3.5, +3.5}, //right,top,front
	{+3.5, +3.5, -3.5}, //right,top,back
	{-3.5, +3.5, -3.5}, //left, top,back
	{-3.5, -3.5, +3.5}, //left,bottom,front
	{+3.5, -3.5, +3.5}, //right,bottom,front
	{+3.5, -3.5, -3.5}, //right,bottom,back
	{-3.5, -3.5, -3.5}  //left,bottom,back
  };

  // This defines the colour of each vertex.
  float   vertexColDat[8][3]=
  {
	{0.5, 0.0, 0.0}, //dark red
	{0.3, 0.3, 0.6}, //dark blue
	{1.0, 0.0, 0.0}, //red
	{0.3, 0.3, 0.6}, //dark blue
	{0.3, 0.3, 0.6}, //dark blue
	{0.5, 0.0, 0.0}, //dark red
	{0.3, 0.3, 0.6}, //dark blue
	{1.0, 0.0, 0.0}, //red
  };

  // This defines the vertexes of each quad in anti-clockwise order.
	unsigned int quadVerDat[6][4]=
	{
	  {0,1,2,3}, //top
	  {0,3,7,4}, //left
	  {3,2,6,7}, //back
	  {2,1,5,6}, //right
	  {0,4,5,1}, //front
	  {4,7,6,5}, //bottom
	};

	int a,b;

	 // Put the vertex data into the cube.ver[] struct.
	 for (a=0;a<8;++a)
	 {
	   for (b=0;b<3;++b)
	   {
		 cube.ver[a].pos[b]=vertexPosDat[a][b];
		 cube.ver[a].col[b]=vertexColDat[a][b];
	   }
	 }

	 // Put the quad data into the cube.quad[] struct.
	 for (a=0;a<6;++a)
	 {
	   for (b=0;b<4;++b)
	   {
		 cube.quad[a].ver[b]=quadVerDat[a][b];
	   }
	 }
}

}
}
}
