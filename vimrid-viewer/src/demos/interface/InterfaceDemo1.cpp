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

#include "InterfaceDemo1.h"

using namespace vimrid::input;

namespace vimrid
{
namespace demos
{
namespace interface
{

InterfaceDemo1::InterfaceDemo1(VimridSettings &settings)
	: GlutApplication(settings)
{
	Title = "Interface demo 1 - Tue 2 Apr";
	EnableCursor = true;
	RenderCameraPosition = true;
	CameraTrim.Z = -20;
}

InterfaceDemo1::~InterfaceDemo1()
{
}

void InterfaceDemo1::Init()
{
	this->GlutApplication::Init();

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	createCube();
}

void InterfaceDemo1::Update()
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
}

void InterfaceDemo1::Render()
{
	this->GlutApplication::Render();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glTranslatef(0, 0, CameraTrim.Z + CameraPoint.Z);
	glRotatef(CameraTrim.Y + CameraPoint.Y, 1, 0, 0);
	glRotatef(-(CameraTrim.X + CameraPoint.X), 0, 1, 0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	drawCube();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// Translate just this matrix so that the menu edges are visible.
	glTranslatef(0, 0, -4);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// Use a dark gray background and set transparency.
	glColor4f(1.0, 1.0, 1.0, 0.5);

	glBegin(GL_QUADS);
	glVertex3f(-1.0, +1.0, 0.0); // top left
	glVertex3f(-1.0, -1.0, 0.0); // bottom left
	glVertex3f(+1.0, -1.0, 0.0); // bottom right
	glVertex3f(+1.0, +1.0, 0.0); // top right
	glEnd();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	FinishRender();
}

void InterfaceDemo1::Reshape()
{
	this->GlutApplication::Reshape();

	glViewport(0, 0, (GLsizei)GetWindowSize().Width, (GLsizei)GetWindowSize().Height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, (GLfloat)GetWindowSize().Width / (GLfloat)GetWindowSize().Height, 1.0, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void InterfaceDemo1::drawCube()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBegin(GL_QUADS);
	for (int a = 0; a < 6; ++a)
	{
		for (int b = 0; b < 4; ++b)
		{
			int currentVer = cube.quad[a].ver[b];
			glColor4fv(cube.ver[currentVer].col);
			glVertex3fv(cube.ver[currentVer].pos);
		}
	}
	glEnd();
}

void InterfaceDemo1::createCube()
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
  float   vertexColDat[8][4]=
  {
	{0.5, 0.0, 0.0, 1.0}, //dark red
	{0.3, 0.3, 0.6, 1.0}, //dark blue
	{1.0, 0.0, 0.0, 1.0}, //red
	{0.3, 0.3, 0.6, 1.0}, //dark blue
	{0.3, 0.3, 0.6, 1.0}, //dark blue
	{0.5, 0.0, 0.0, 1.0}, //dark red
	{0.3, 0.3, 0.6, 1.0}, //dark blue
	{1.0, 0.0, 0.0, 1.0}, //red
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
