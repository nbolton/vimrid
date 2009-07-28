/* This version uses very crude stereo and trackd implementations. Here I'm using the
 * easy, but not technically correct method of displaying stereo images (simply, an X
 * transformation) which shows the same image in each eye (just slightly offset). And,
 * a quick implementation of the See3D trackd API shows that it can recieve coordinates
 * from the motion sensors, but this isn't conveyed to the user in a sensible way;
 * partialy to blame here is that the camera isn't actually moving - it just appears
 * that way because the cube rotates based on camera movement. This is great for demo
 * purposes but has some flaws; one is that the rotation only makes sense when
 * the cube is rotated in just 1 direction from it's default position. */

#include "CubeDemo1.h"
#include <iostream>

using namespace std;
using namespace vimrid::input;

namespace vimrid
{
namespace demos
{
namespace cubedemos
{

CubeDemo1::CubeDemo1(VimridSettings &settings)
	: GlutApplication(settings)
{
	Title = "ViMRID cube demo - v1 - 04/02/09";

	DepthPerception = .8;
	CameraTrim.Z = -50;
	SetWindowSize(800, 800);

	if (GetInputManager()->IsTrackdSupported())
	{
		GetInputManager()->InitTrackd(TD_TRACKER_KEY);
		GetInputManager()->AddTrackdSensor(TD_GLASSES_ID);
		GetInputManager()->AddTrackdSensor(TD_WAND_ID);
	}
}

void CubeDemo1::Init()
{
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STEREO);
	glutInitWindowSize((int)GetWindowSize().Width, (int)GetWindowSize().Height);
	glutCreateWindow(GetSettings().DisplayName.c_str());

	glMatrixMode(GL_PROJECTION);
	gluPerspective(45, 1, 10, 200);
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_CULL_FACE);

	createTestCube();
}

void CubeDemo1::Update()
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

void CubeDemo1::Render()
{
	glDrawBuffer(GL_BACK_LEFT);
	drawCube(+DepthPerception, -5 + CameraPoint.Y * .1, CameraPoint.Z, CameraPoint.X * -1, 0, 0);

	glDrawBuffer(GL_BACK_RIGHT);
	drawCube(-DepthPerception, -5 + CameraPoint.Y * .1, CameraPoint.Z, CameraPoint.X * -1, 0, 0);

	glutSwapBuffers();
	glutPostRedisplay();
}

void CubeDemo1::drawCube(
	float translateX, float translateY, float translateZ,
	float rotateX, float rotateY, float rotateZ)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glTranslatef(translateX, translateY, translateZ);
	glRotatef(rotateX, 0, 1, 0);
	glRotatef(rotateY, 1, 0, 0);

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

void CubeDemo1::updateCamera()
{
	// Apply any required trimming to camera position.
	CameraPoint.X = CameraTrim.X;
	CameraPoint.Y = CameraTrim.Y;
	CameraPoint.Z = CameraTrim.Z;

	// Here, the mouse controls the rotation and translation of the cube (camera never moves).
	const MouseInfo &mouse = GetInputManager()->GetMouseInfo();
	CameraPoint.X += (mouse.GetPosition().X * 0.15);
	CameraPoint.Y += (mouse.GetPosition().Y * 0.1);

	if (GetInputManager()->IsTrackdSupported())
	{
		/* The glasses position info will be reflected on translation and rotation of
		 * cube, so the camera never actually moves (it just seems like it does). */
		const TrackdSensorInfo &glasses = GetInputManager()->GetTrackdSensorInfo(TD_GLASSES_ID);
		CameraPoint.X += glasses.GetPosition().X * 10;
		CameraPoint.Y += ((glasses.GetPosition().Y * -1) * 10.5);
		CameraPoint.Z += glasses.GetPosition().Z * 10;
		cout << "Camera X: " << CameraPoint.X
			<< " Y: " << CameraPoint.Y
			<< " Z: " << CameraPoint.Z << endl;
	}
}

void CubeDemo1::Resized()
{
	// TODO: change this so that the aspect ratio is not altered
	glViewport(0, 0, (int)GetWindowSize().Width, (int)GetWindowSize().Height);
}

/* This tutorial is Copyright © 2001 Ben Woodhouse
 * http://www.gamedev.net/reference/articles/article1681.asp
 */
void CubeDemo1::createTestCube()
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

}
}
}
