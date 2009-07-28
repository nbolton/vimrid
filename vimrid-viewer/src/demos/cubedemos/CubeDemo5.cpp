/* This demo will be written in SDL (not platform independant), so
 * will only work with Linux. Going to base this on the code structure
 * used in the glxgears demo. */

#include "CubeDemo5.h"
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

CubeDemo5::CubeDemo5(VimridSettings &settings)
	: SdlApplication(settings)
{
	Title = "ViMRID Test Cube - v5 - 20/02/09";
	SetWindowSize(800, 800);

	viewRotateX = 20.0;
	viewRotateY = 30.0;
	viewRotateZ = 0.0;

	angle = 0.0;
	eyesep = 5.0;
	fix_point = 40.0;

	viewRotateXVelocity = 0;
	viewRotateYVelocity = 0;
}

void CubeDemo5::Init()
{
	this->SdlApplication::Init();

	static GLfloat pos[4] = { 5.0, 5.0, 10.0, 0.0 };
	static GLfloat red[4] = { 0.8, 0.1, 0.0, 1.0 };
	static GLfloat green[4] = { 0.0, 0.8, 0.2, 1.0 };
	static GLfloat blue[4] = { 0.2, 0.2, 1.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	/* make the gears */
	gear1 = glGenLists(1);
	glNewList(gear1, GL_COMPILE);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
	createGear(1.0, 4.0, 1.0, 20, 0.7);
	glEndList();

	gear2 = glGenLists(1);
	glNewList(gear2, GL_COMPILE);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
	createGear(0.5, 2.0, 2.0, 10, 0.7);
	glEndList();

	gear3 = glGenLists(1);
	glNewList(gear3, GL_COMPILE);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
	createGear(1.3, 2.0, 0.5, 10, 0.7);
	glEndList();

	glEnable(GL_NORMALIZE);
}

void CubeDemo5::Update()
{
	this->SdlApplication::Update();

	bool keyPressed = false;
	if (IsKeyPressed(XK_Left))
	{
		keyPressed = true;
		if (viewRotateYVelocity < maxRotateVelocity)
		{
			viewRotateYVelocity += .5;
		}
	}
	else if (IsKeyPressed(XK_Right))
	{
		keyPressed = true;
		if (viewRotateYVelocity > (maxRotateVelocity * -1))
		{
			viewRotateYVelocity -= .5;
		}
	}
	else if (IsKeyPressed(XK_Up))
	{
		keyPressed = true;
		if (viewRotateXVelocity < maxRotateVelocity)
		{
			viewRotateXVelocity += .5;
		}
	}
	else if (IsKeyPressed(XK_Down))
	{
		keyPressed = true;
		if (viewRotateXVelocity > (maxRotateVelocity * -1))
		{
			viewRotateXVelocity -= .5;
		}
	}
	else if (IsKeyPressed(XK_Escape) || IsKeyPressed(XK_q) || IsKeyPressed(XK_Q))
	{
		Exit();
	}

	if (!keyPressed)
	{
		// Decay the rotation gradually (only when input not recieved).
		viewRotateXVelocity *= 0.999;
		viewRotateYVelocity *= 0.999;
	}

	// Keep rotating, only while there is still velocity.
	viewRotateX += viewRotateXVelocity * (GetUtility().GetDeltaTime() * .01);
	viewRotateY += viewRotateYVelocity * (GetUtility().GetDeltaTime() * .01);

	// Rotate the gears based on delta time (so speed is universal).
	angle += .01 * GetUtility().GetDeltaTime();

	/* When the angle comes around full circle, reset back
	 * to zero (as its the same as 360 anyway), so that the
	 * angle doesn't increase infinately. */
	if (angle == 360)
	{
		angle = 0;
	}
}

void CubeDemo5::Render()
{
	this->SdlApplication::Render();

	if (GetSettings().EnableStereo)
	{
		// First left eye.
		glDrawBuffer(GL_BACK_LEFT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(left, right, -asp, asp, 5.0, 60.0);

		glMatrixMode(GL_MODELVIEW);

		glPushMatrix();
		glTranslated(+0.5 * eyesep, 0.0, 0.0);
		drawGears();
		glPopMatrix();

		// Then right eye.
		glDrawBuffer(GL_BACK_RIGHT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-right, -left, -asp, asp, 5.0, 60.0);

		glMatrixMode(GL_MODELVIEW);

		glPushMatrix();
		glTranslated(-0.5 * eyesep, 0.0, 0.0);
		drawGears();
		glPopMatrix();
	}
	else
	{
		drawGears();
	}

	FinishRender();
}

void CubeDemo5::Reshape()
{
	this->SdlApplication::Reshape();

	glViewport(0, 0, (GLint)GetWindowSize().Width, (GLint)GetWindowSize().Height);

	if (GetSettings().EnableStereo)
	{
		asp = (GLfloat)GetWindowSize().Height / (GLfloat)GetWindowSize().Width;
		GLfloat w = fix_point * (1.0 / 5.0);

		left = -5.0 * ((w - 0.5 * eyesep) / fix_point);
		right = 5.0 * ((w + 0.5 * eyesep) / fix_point);
	}
	else
	{
		GLfloat h = (GLfloat)GetWindowSize().Height / (GLfloat)GetWindowSize().Width;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-1.0, 1.0, -h, h, 5.0, 60.0);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -40.0);
}

void CubeDemo5::drawGears()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glRotatef(viewRotateX, 1.0, 0.0, 0.0);
	glRotatef(viewRotateY, 0.0, 1.0, 0.0);
	glRotatef(viewRotateZ, 0.0, 0.0, 1.0);

	glPushMatrix();
	glTranslatef(-3.0, -2.0, 0.0);
	glRotatef(angle, 0.0, 0.0, 1.0);
	glCallList(gear1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(3.1, -2.0, 0.0);
	glRotatef(-2.0 * angle - 9.0, 0.0, 0.0, 1.0);
	glCallList(gear2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-3.1, 4.2, 0.0);
	glRotatef(-2.0 * angle - 25.0, 0.0, 0.0, 1.0);
	glCallList(gear3);
	glPopMatrix();

	glPopMatrix();
}

void CubeDemo5::createGear(
	GLfloat innerRadius,
	GLfloat outerRadius,
	GLfloat width,
	GLint teeth,
	GLfloat toothDepth)
{
	GLint i;
	GLfloat r0, r1, r2;
	GLfloat angle, da;
	GLfloat u, v, len;

	r0 = innerRadius;
	r1 = outerRadius - toothDepth / 2.0;
	r2 = outerRadius + toothDepth / 2.0;

	da = 2.0 * M_PI / teeth / 4.0;

	glShadeModel(GL_FLAT);

	glNormal3f(0.0, 0.0, 1.0);

	/* draw front face */
	glBegin(GL_QUAD_STRIP);
	for (i = 0; i <= teeth; i++)
	{
		angle = i * 2.0 * M_PI / teeth;
		glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
		glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
		if (i < teeth) {
	 glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
	 glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da),
			 width * 0.5);
		}
	}
	glEnd();

	/* draw front sides of teeth */
	glBegin(GL_QUADS);
	da = 2.0 * M_PI / teeth / 4.0;
	for (i = 0; i < teeth; i++) {
		angle = i * 2.0 * M_PI / teeth;

		glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
		glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5);
		glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da),
		 width * 0.5);
		glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da),
		 width * 0.5);
	}
	glEnd();

	glNormal3f(0.0, 0.0, -1.0);

	/* draw back face */
	glBegin(GL_QUAD_STRIP);
	for (i = 0; i <= teeth; i++) {
		angle = i * 2.0 * M_PI / teeth;
		glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
		glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
		if (i < teeth) {
	 glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da),
			 -width * 0.5);
	 glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
		}
	}
	glEnd();

	/* draw back sides of teeth */
	glBegin(GL_QUADS);
	da = 2.0 * M_PI / teeth / 4.0;
	for (i = 0; i < teeth; i++) {
		angle = i * 2.0 * M_PI / teeth;

		glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da),
		 -width * 0.5);
		glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da),
		 -width * 0.5);
		glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5);
		glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
	}
	glEnd();

	/* draw outward faces of teeth */
	glBegin(GL_QUAD_STRIP);
	for (i = 0; i < teeth; i++) {
		angle = i * 2.0 * M_PI / teeth;

		glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
		glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
		u = r2 * cos(angle + da) - r1 * cos(angle);
		v = r2 * sin(angle + da) - r1 * sin(angle);
		len = sqrt(u * u + v * v);
		u /= len;
		v /= len;
		glNormal3f(v, -u, 0.0);
		glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5);
		glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5);
		glNormal3f(cos(angle), sin(angle), 0.0);
		glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da),
		 width * 0.5);
		glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da),
		 -width * 0.5);
		u = r1 * cos(angle + 3 * da) - r2 * cos(angle + 2 * da);
		v = r1 * sin(angle + 3 * da) - r2 * sin(angle + 2 * da);
		glNormal3f(v, -u, 0.0);
		glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da),
		 width * 0.5);
		glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da),
		 -width * 0.5);
		glNormal3f(cos(angle), sin(angle), 0.0);
	}

	glVertex3f(r1 * cos(0.0), r1 * sin(0.0), width * 0.5);
	glVertex3f(r1 * cos(0.0), r1 * sin(0.0), -width * 0.5);

	glEnd();

	glShadeModel(GL_SMOOTH);

	/* draw inside radius cylinder */
	glBegin(GL_QUAD_STRIP);
	for (i = 0; i <= teeth; i++) {
		angle = i * 2.0 * M_PI / teeth;
		glNormal3f(-cos(angle), -sin(angle), 0.0);
		glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
		glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
	}
	glEnd();
}

}
}
}
