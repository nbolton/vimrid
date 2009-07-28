/* Following from cube demo 1, this version improves on the implementation of trackd
 * by using the gluLookAt function, which always focuses on a single point (assuming
 * that the user is always looking at the screen) but moves the camera to the location
 * of the glasses motion sensor. This demo still uses the 'simple' method of creating
 * a stereo image (as is used in cube demo 1). */

#include "CubeDemo2.h"
#include <iostream>

using namespace std;
using namespace vimrid::input;

namespace vimrid
{
namespace demos
{
namespace cubedemos
{

CubeDemo2::CubeDemo2(VimridSettings &settings)
	: GlutApplication(settings)
{
	Title = "ViMRID cube demo - v2 - 11/02/09";

	DepthPerception = 2;
	SetWindowSize(800, 800);

	if (GetInputManager()->IsTrackdSupported())
	{
		GetInputManager()->InitTrackd(TD_TRACKER_KEY);
		GetInputManager()->AddTrackdSensor(TD_GLASSES_ID);
		GetInputManager()->AddTrackdSensor(TD_WAND_ID);
	}
}

void CubeDemo2::Init()
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

void CubeDemo2::Update()
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

void CubeDemo2::Render()
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

void CubeDemo2::drawCube(unsigned int buffer, float depthPerception)
{
	glDrawBuffer(buffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	/* Phil said that the easy way of creating depth perception is by translating
	 * X on each buffer, but the correct way is actually to create a new frustum
	 * (for each eye?) */
	glTranslatef(depthPerception, 0, 0);

	// always use the same look point (as the feature never moves), but move the
	// eye to where the camera is (i.e. mouse or motion sensor)
	gluLookAt(CameraPoint.X, CameraPoint.Y, CameraPoint.Z, 0, 0, 0, 0, 1, 0);

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

void CubeDemo2::Resized()
{
	glViewport(0, 0, (int)GetWindowSize().Width, (int)GetWindowSize().Height);
}

/* This tutorial is Copyright © 2001 Ben Woodhouse
 * http://www.gamedev.net/reference/articles/article1681.asp
 */
void CubeDemo2::createTestCube()
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

void CubeDemo2::updateCamera()
{
	if (GetInputManager()->IsTrackdSupported())
	{
		const TrackdSensorInfo &glasses = GetInputManager()->GetTrackdSensorInfo(TD_GLASSES_ID);
		CameraPoint.X = (glasses.GetPosition().X) * 20;
		CameraPoint.Y = 100 + ((glasses.GetPosition().Z) * 30);
		CameraPoint.Z = 100 + (100 - ((glasses.GetPosition().Y) * 10));
	}
	else
	{
		// When mouse moves up and down, move toward/away from screen.
		const MouseInfo &mouse = GetInputManager()->GetMouseInfo();
		CameraPoint.X = ((-200 + mouse.GetPosition().X) * 0.15);
		CameraPoint.Z = 50 + (mouse.GetPosition().Y) * 0.2;
	}

	cout << "Camera X: " << CameraPoint.X
		<< " Y: " << CameraPoint.Y
		<< " Z: " << CameraPoint.Z << endl;
}

}
}
}
