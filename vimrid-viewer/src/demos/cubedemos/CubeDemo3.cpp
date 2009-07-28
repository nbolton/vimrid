/* In this demo, I attempted to do two things:
 * - improve on the previous method of producing a stereo image;
 * - and, allow the user to rotate the cube using the wand motion sensor.
 *
 * My new stero theory seemed logical (and to those who I had described it, also
 * shared my understanding); simply change the paralax of each eye from focusing straight
 * ahead (where each eye is parallel, as in the previous demos) to instead focus at
 * the object from two different angles - just like your eyes do normally.
 *
 * I later discovered that this is known as toe-in or toed-in stereo method, which
 * is not as correct as the off-axis or "asymetric fustrum parallel axis projection
 * stereo" method (which uses glFrustum) as described next...
 *
 * However, the demo prooved that this just confuses the user's eyes/brain, and
 * produces a weird twisted shape on the object, and no real depth perception. Phil Summers
 * (at See3D) kindly pointed out that while it seems counter-intuative, this method
 * is wrong, and the most technically correct method is to create a new
 * frustum for each eye, presumably by using the glFrustum function (as can be seen
 * in the glxgears demo) - aka off-axis.
 *
 * This is what I intend on doing in demo 4 (use glFrustum), as well as improving the
 * object rotation caused by movement of the wand motion sensor. */

#include "CubeDemo3.h"
#include <iostream>

using namespace std;
using namespace vimrid::input;

namespace vimrid
{
namespace demos
{
namespace cubedemos
{

CubeDemo3::CubeDemo3(VimridSettings &settings)
	: GlutApplication(settings)
{
	Title = "ViMRID cube demo - v3 - 13/02/09";

	DepthPerception = 2;
	SetWindowSize(800, 800);
	if (GetInputManager()->IsTrackdSupported())
	{
		GetInputManager()->InitTrackd(TD_TRACKER_KEY);
		GetInputManager()->AddTrackdSensor(TD_GLASSES_ID);
		GetInputManager()->AddTrackdSensor(TD_WAND_ID);
	}
}

void CubeDemo3::Init()
{
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STEREO);
	glutInitWindowSize((int)GetWindowSize().Width, (int)GetWindowSize().Height);
	glutCreateWindow(GetSettings().DisplayName.c_str());

	glMatrixMode(GL_PROJECTION);
	gluPerspective(45, 1, 10, 450);
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_CULL_FACE);

	createTestCube();
}

void CubeDemo3::Update()
{
	if (IsSpecialKeyPressed(GLUT_KEY_UP))
	{
		CameraTrim.Z += .05;
	}

	if (IsSpecialKeyPressed(GLUT_KEY_DOWN))
	{
		CameraTrim.Z -= .05;
	}

	if (IsSpecialKeyPressed(GLUT_KEY_PAGE_UP))
	{
		DepthPerception += .005;
	}

	if (IsSpecialKeyPressed(GLUT_KEY_PAGE_DOWN))
	{
		DepthPerception += -.005;
	}

	updateCamera();
}

void CubeDemo3::Render()
{
	drawCube(GL_BACK_LEFT, +DepthPerception);
	drawCube(GL_BACK_RIGHT, -DepthPerception);

	Point2f* rbsPoint = new Point2f();
	rbsPoint->X = -10;
	rbsPoint->Y = -10;
	RenderStringOrtho2D(*rbsPoint, GLUT_BITMAP_HELVETICA_18, "Hello world!");

	glutSwapBuffers();
	glutPostRedisplay();
}

void CubeDemo3::drawCube(unsigned int buffer, float depthPerception)
{
	glDrawBuffer(buffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	// This method is wrong, see comments for details.
	gluLookAt(depthPerception + CameraPoint.X, CameraPoint.Y, CameraPoint.Z, 0, 0, 0, 0, 1, 0);

	// Rotate the cube depending on the wand motion sensor.
	glRotatef(FeatureRotation.X, 1, 0, 0);
	glRotatef(FeatureRotation.Y, 0, 1, 0);
	glRotatef(FeatureRotation.Z, 0, 0, 1);

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

	glPopMatrix();
}

void CubeDemo3::Resized()
{
	// TODO: change this so that the aspect ratio is not altered
	glViewport(0, 0, (int)GetWindowSize().Width, (int)GetWindowSize().Height);
}

/* This tutorial is Copyright © 2001 Ben Woodhouse
 * http://www.gamedev.net/reference/articles/article1681.asp
 */
void CubeDemo3::createTestCube()
{
	//defines the position of each vertex
  float  vertexPosDat[8][3]=
  {
	{-10, 10, 10},  //left,top,front
	{10, 10, 10},   //right,top,front
	{10, 10,-10},   //right,top,back
	{-10, 10,-10},  //left, top,back
	{-10,-10, 10},  //left,bottom,front
	{10, -10, 10},  //right,bottom,front
	{10, -10,-10},  //right,bottom,back
	{-10,-10,-10}   //left,bottom,back
  };

  //defines the colour of each vertex
  float   vertexColDat[8][3]=
  {
	{0.5, 0 ,0},   //dark red
	{1, 1, 0.3},   //yellow
	{1, 0, 0},     //red
	{0.5, 1, 0.2}, //dull yellow??
	{1, 1, 0},     //yellow
	{0.9, 0.5, 0}, //orange
	{1, 0.9, 0.1}, //yellow
	{1, 0, 0},     //red
  };

  //defines the vertexes of each quad in anti-clockwise order
	unsigned int quadVerDat[6][4]=
	{
	  {0,1,2,3},  //top
	  {0,3,7,4},  //left
	  {3,2,6,7},  //back
	  {2,1,5,6},  //right
	  {0,4,5,1},  //front
	  {4,7,6,5},  //bottom
	};

	int a,b;

	 //put the vertex data into the cube.ver[] struct
	 for (a=0;a<8;++a)
	 {
	   for (b=0;b<3;++b)
	   {
		 cube.ver[a].pos[b]=vertexPosDat[a][b];
		 cube.ver[a].col[b]=vertexColDat[a][b];
	   }
	 }

	 //put the quad data into the cube.quad[] struct
	 for (a=0;a<6;++a)
	 {
	   for (b=0;b<4;++b)
	   {
		 cube.quad[a].ver[b]=quadVerDat[a][b];
	   }
	 }
}

void CubeDemo3::updateCamera()
{
	if (GetInputManager()->IsTrackdSupported())
	{
		// Move the camera to where the glasses are.
		const TrackdSensorInfo &glasses = GetInputManager()->GetTrackdSensorInfo(TD_GLASSES_ID);
		CameraPoint.X = (glasses.GetPosition().X) * 20;
		CameraPoint.Y = 100 + ((glasses.GetPosition().Z) * 30);
		CameraPoint.Z = 100 + (100 - ((glasses.GetPosition().Y) * 10));

		// Allow the user to "grab" and rotate the object.
		// TODO: Detect wand button states, and only move when one is pressed.
		const TrackdSensorInfo &wand = GetInputManager()->GetTrackdSensorInfo(TD_WAND_ID);
		FeatureRotation.X = wand.GetPosition().X * 10;
		FeatureRotation.Y = wand.GetPosition().Z * 10;
		FeatureRotation.Z = (wand.GetPosition().Y * -1) * 10;
	}
	else
	{
		// When mouse moves up and down, move toward/away from screen.
		const MouseInfo &mouse = GetInputManager()->GetMouseInfo();
		CameraPoint.X = ((-200 + mouse.GetPosition().X) * 0.15);
		CameraPoint.Z = 50 + (mouse.GetPosition().Y) * 0.2;
	}

	/*cout << "Camera            X: " << CameraPoint.X
		<< " Y: " << CameraPoint.Y
		<< " Z: " << CameraPoint.Z << endl;*/

	cout << "Featire Rotation  X: " << FeatureRotation.X
		<< " Y: " << FeatureRotation.Y
		<< " Z: " << FeatureRotation.Z << endl;

}

}
}
}
