/* In this demo, the I have converted CubeDemo5 which is SDL, to GLUT. This
 * was actually quite easy as I had already abstracted most of the window
 * management and input stuff to the base classes (which are essentially just
 * wrappers for SDL and GLUT).
 *
 * This is the first successful demo of stereo using the ViMRID application
 * architecture and GLUT. Credit for the indices and stereo geometry goes to
 * the author of glxgears.c (Brian Paul).
 */

#include "CubeDemo6.h"
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

CubeDemo6::CubeDemo6(VimridSettings &settings)
	: GlutApplication(settings)
{
	Title = "ViMRID Test Cube - v6 - 21/02/09";

	PrintFps = true;
	SetWindowSize(800, 800);

	viewRotateX = 20.0;
	viewRotateY = 30.0;
	viewRotateZ = 0.0;

	gearRotateAngle = 0.0;
	stereoEyeSeparation = 5.0;
	fixationPoint = 40.0;

	viewRotateXVelocity = 0.0;
	viewRotateYVelocity = 0.0;

	frustumNearClip = 5.0;
	frustumFarClip = 60.0;
}

void CubeDemo6::Init()
{
	this->GlutApplication::Init();

	static float pos[4] = { 5.0, 5.0, 10.0, 0.0 };
	static float red[4] = { 0.8, 0.1, 0.0, 1.0 };
	static float green[4] = { 0.0, 0.6, 0.2, 1.0 };
	static float blue[4] = { 0.3, 0.3, 0.6, 1.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

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

void CubeDemo6::Update()
{
	this->GlutApplication::Update();

	bool keyPressed = false;
	if (IsSpecialKeyPressed(GLUT_KEY_LEFT))
	{
		keyPressed = true;
		if (viewRotateYVelocity < maxRotateVelocity)
		{
			viewRotateYVelocity += (GetUtility().GetDeltaTime() * .005);
		}
	}
	else if (IsSpecialKeyPressed(GLUT_KEY_RIGHT))
	{
		keyPressed = true;
		if (viewRotateYVelocity > (maxRotateVelocity * -1))
		{
			viewRotateYVelocity -= (GetUtility().GetDeltaTime() * .005);
		}
	}
	else if (IsSpecialKeyPressed(GLUT_KEY_UP))
	{
		keyPressed = true;
		if (viewRotateXVelocity < maxRotateVelocity)
		{
			viewRotateXVelocity += (GetUtility().GetDeltaTime() * .005);
		}
	}
	else if (IsSpecialKeyPressed(GLUT_KEY_DOWN))
	{
		keyPressed = true;
		if (viewRotateXVelocity > (maxRotateVelocity * -1))
		{
			viewRotateXVelocity -= (GetUtility().GetDeltaTime() * .005);
		}
	}
	else if (/*IsSpecialKeyPressed(GLUT_KEY_ESCAPE) || */
			IsCharKeyPressed('q') ||
			IsCharKeyPressed('Q'))
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
	gearRotateAngle += .01 * GetUtility().GetDeltaTime();

	/* When the angle comes around full circle, reset back
	 * to zero (as its the same as 360 anyway), so that the
	 * angle doesn't increase infinately. */
	if (gearRotateAngle == 360)
	{
		gearRotateAngle = 0;
	}
}

void CubeDemo6::drawStereoEyeView(GLenum eyeBuffer)
{
	// What does w stand for? Author of glxgears is to blame!
	float w = fixationPoint * (1.0 / 5.0);

	/* These could be altered when the window is resized, however there is
	 * no performance hit when run in this class, so it makes more sense to
	 * run them here, so that it's more obvious how the frustum is generated. */
	double frustumLeft = -5.0 * ((w - 0.5 * stereoEyeSeparation) / fixationPoint);
	double frustumRight = 5.0 * ((w + 0.5 * stereoEyeSeparation) / fixationPoint);

	/* Each left and right eye must be displayed on either side
	 * of the central view, so where right is -1, left should be +1 */
	int eyeSideFactor = (eyeBuffer == GL_BACK_LEFT) ? +1 : -1;

	glDrawBuffer(eyeBuffer);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	/* The left and right frustums are swapped for each eye;
	 * if this didnt happen, the viewer would be looking at the
	 * objects from the back (in the left eye) and the front (from
	 * the right eye). This applies to the right eye's frustum, as
	 * you can see the frustumRight variable is past to the first
	 * parameter (which is actually called left) - this could be
	 * misinterprated as a mistake, as it seems counter-intuative.
	 */
	switch(eyeBuffer)
	{
		case GL_BACK_LEFT:
			glFrustum(
					(frustumLeft * eyeSideFactor),
					(frustumRight * eyeSideFactor),
					-aspectRatio, aspectRatio,
					frustumNearClip, frustumFarClip);
			break;

		case GL_BACK_RIGHT:
			glFrustum(
					(frustumRight * eyeSideFactor),
					(frustumLeft * eyeSideFactor),
					-aspectRatio, aspectRatio,
					frustumNearClip, frustumFarClip);
			break;
	}

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	/* After the frustum for either eye has been set up, we now need to
	 * move each eye in the same direction of the frustum so that the frustum
	 * planes of each eye are properly aligned. */
	glTranslated((0.5 * eyeSideFactor) * stereoEyeSeparation, 0.0, 0.0);

	// Now draw the feature (once for each eye).
	drawGears();

	glPopMatrix();
}

void CubeDemo6::Render()
{
	this->GlutApplication::Render();

	if (GetSettings().EnableStereo)
	{
		/* Set the background to a slightly gray/off-black colour
		 * so that the shadowing of the stereo effect is reduced,
		 * due to the colours not contrasting as badly as if it
		 * were solid black. */
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

		drawStereoEyeView(GL_BACK_LEFT);
		drawStereoEyeView(GL_BACK_RIGHT);
	}
	else
	{
		drawGears();
	}

	FinishRender();
}

void CubeDemo6::Reshape()
{
	this->GlutApplication::Reshape();

	glViewport(0, 0, (int)GetWindowSize().Width, (int)GetWindowSize().Height);

	aspectRatio = (float)GetWindowSize().Height / (float)GetWindowSize().Width;

	if (!GetSettings().EnableStereo)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-1.0, 1.0, -aspectRatio, aspectRatio, 5.0, 60.0);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -40.0);
}

void CubeDemo6::drawGears()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glRotatef(viewRotateX, 1.0, 0.0, 0.0);
	glRotatef(viewRotateY, 0.0, 1.0, 0.0);
	glRotatef(viewRotateZ, 0.0, 0.0, 1.0);

	glPushMatrix();
	glTranslatef(-3.0, -2.0, 0.0);
	glRotatef(gearRotateAngle, 0.0, 0.0, 1.0);
	glCallList(gear1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(3.1, -2.0, 0.0);
	glRotatef(-2.0 * gearRotateAngle - 9.0, 0.0, 0.0, 1.0);
	glCallList(gear2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-3.1, 4.2, 0.0);
	glRotatef(-2.0 * gearRotateAngle - 25.0, 0.0, 0.0, 1.0);
	glCallList(gear3);
	glPopMatrix();

	glPopMatrix();
}

void CubeDemo6::createGear(
	float innerRadius,
	float outerRadius,
	float width,
	int teeth,
	float toothDepth)
{
	int i;
	float r0, r1, r2;
	float angle, da;
	float u, v, len;

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
