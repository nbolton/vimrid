/*
 * ViMRID - Visualisation for Magnetic Resonance Imaging data
 * A medical visualisation tool for students of oncology
 * njb4@aber.ac.uk - Aberystwyth University
 * Copyright (C) 2009  Nick Bolton
 */

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include "GlutApplication.h"
#include <sstream>
#include <pthread.h>
#include "../input/DragMode.h"

using namespace std;
using namespace vimrid::input;

#define GA_GAME_MODE_STRING "1280x960:32@85"

namespace vimrid
{
namespace glut
{

// The default display mode is double-buffered stereo.
// Hide the cursor by default (as not to destroy stereo illusion).
// Don't show the FPS on screen by default.
GlutApplication::GlutApplication(VimridSettings &settings) :
	VimridApplication(settings, new GlutInputManager()),
	DepthPerception(true),
	CameraPoint(),
	CameraTrim(),
	FeatureRotation(),
	EnableCursor(false),
	RenderFps(false),
	RenderCameraPosition(false),
	RenderDragInfo(false),
	RenderMouseInfo(false),
	RenderStatusTextOnFinish(true),
	EnableRenderStatusText(true),
	RenderTrackdInfo(false)
{
}

GlutApplication::~GlutApplication()
{
}

GlutApplication *GlutApplication::GetGlutInstance()
{
	return (GlutApplication*)VimridApplication::GetInstance();
}

GlutInputManager *GlutApplication::GetGlutInputManager()
{
	return (GlutInputManager*)GetInputManager();
}

void GlutApplication::RenderStringOrtho2D(Point2f point, void* font, string s)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, GetWindowSize().Width, 0, GetWindowSize().Height, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(point.X, point.Y, 0);

	glRasterPos2f(0, 0);
	for (string::iterator i = s.begin(); i != s.end(); ++i)
	{
		glutBitmapCharacter(font, *i);
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}

void GlutApplication::Run()
{
	this->VimridApplication::Run();

	glutInit(&GetSettings().ArgumentCount, GetSettings().ArgumentVariables);

	// Allow derived classes to do some init stuff.
	Init();

	// Ensure that we don't get spammed with key down events.
	glutIgnoreKeyRepeat(true);

	// Handle the glut callbacks with static methods.
	glutDisplayFunc(glutCallbackDisplay);
	glutReshapeFunc(glutCallbackReshape);
	glutKeyboardFunc(glutCallbackKeyboard);
	glutKeyboardUpFunc(glutCallbackKeyboardUp);
	glutMouseFunc(glutCallbackMouse);
	glutMotionFunc(glutCallbackMotion);
	glutPassiveMotionFunc(glutCallbackPassiveMotion);
	glutSpecialFunc(glutCallbackSpecial);
	glutSpecialUpFunc(glutCallbackSpecialUp);

	glutMainLoop();
}

void GlutApplication::RenderStatusText()
{
	if (EnableRenderStatusText)
	{
		// Show info using low intensity colour.
		glColor3f(0.6, 0.6, 0.6);

		VInt32 y = 10;
		for (VInt32 i = (renderTexts.size() - 1); i >= 0; i--)
		{
			RenderStringOrtho2D(
				Point2f(10, y),
				GLUT_BITMAP_9_BY_15,
				renderTexts[i]);

			y += 20;
		}
	}
}

void GlutApplication::FinishRender()
{
	if (RenderStatusTextOnFinish)
	{
		RenderStatusText();
	}

	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
}

bool GlutApplication::IsSpecialKeyPressed(int glutSpecialKey)
{
	return GetGlutInputManager()->IsSpecialKeyPressed(glutSpecialKey);
}

bool GlutApplication::IsCharKeyPressed(char charKey)
{
	return GetGlutInputManager()->IsCharKeyPressed(charKey);
}

bool GlutApplication::IsModiferKeyPressed(int glutModifierKey)
{
	return GetGlutInputManager()->IsModifierKeyPressed(glutModifierKey);
}

VUInt32 GlutApplication::GetDisplayMode() const
{
	VUInt32 mode = GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH;
	if (IsStereoEnabled())
	{
		mode |= GLUT_STEREO;
	}
	return mode;
}

void GlutApplication::Init()
{
	this->VimridApplication::Init();

	glutInitDisplayMode((unsigned int)GetDisplayMode());
	glutInitWindowSize((int)GetWindowSize().Width, (int)GetWindowSize().Height);
	glutCreateWindow(Title.c_str());

	if (GetSettings().FullScreen)
	{
		enterFullScreen();
	}

	if (!EnableCursor)
	{
		glutSetCursor(GLUT_CURSOR_NONE);
	}
}

void GlutApplication::enterFullScreen()
{
	glutGameModeString(GA_GAME_MODE_STRING);
	glutEnterGameMode();
	mFullScreenActive = true;
}

void GlutApplication::leaveFullScreen()
{
	glutLeaveGameMode();
	mFullScreenActive = false;
}

void GlutApplication::Render()
{
}

void GlutApplication::Update()
{
	setupStatusLines();
}

void GlutApplication::setupStatusLines()
{
	renderTexts.clear();

	// Render the FPS just before the buffer is cleared.
	if (RenderFps && (FramesPerSecond != 0))
	{
		stringstream ss;
		ss << "[FPS: " << FramesPerSecond << "] ";
		renderTexts.push_back(ss.str());
	}

	if (RenderMouseInfo)
	{
		stringstream ss;
		const MouseInfo &mouse = GetInputManager()->GetMouseInfo();
		ss << "[Mouse " << mouse.GetPosition();
		ss << " LMB: " << mouse.IsButtonPressed(MOUSE_BUTTON_LEFT);
		ss << " RMB: " << mouse.IsButtonPressed(MOUSE_BUTTON_RIGHT);
		ss << "]";
		renderTexts.push_back(ss.str());
	}

	if (RenderTrackdInfo && GetUtility().IsTrackdSupported())
	{
		stringstream ss;
		const TrackdDeviceInfo &wand = GetUtility().GetTrackdDevice(TD_WAND_ID);
		const TrackdDeviceInfo &glasses = GetUtility().GetTrackdDevice(TD_GLASSES_ID);

		VPoint3f gp = glasses.GetSensor().GetPosition();
		VPoint3f wp = wand.GetSensor().GetPosition();

		ss.setf(ios::showpos);
		ss.setf(ios_base::fixed, ios_base::floatfield);
		ss.precision(2);

		VBool b1 = wand.GetController().IsButtonPressed(TRACKD_BUTTON_1);
		VBool b2 = wand.GetController().IsButtonPressed(TRACKD_BUTTON_2);
		VBool b3 = wand.GetController().IsButtonPressed(TRACKD_BUTTON_3);

		ss << "[Glasses: " << gp << "]";
		renderTexts.push_back(ss.str());

		ss.str("");
		ss << "[Wand: " << wp
			<< " B1: " << (b1 ? "On" : "Off")
			<< " B2: " << (b2 ? "On" : "Off")
			<< " B3: " << (b3 ? "On" : "Off")
			<< "]";
		renderTexts.push_back(ss.str());
	}

	if (RenderDragInfo)
	{
		vector<DragMode*> dragModes = GetDragController().GetModes();
		for (VUInt32 i = 0; i < dragModes.size(); i++)
		{
			stringstream ss;
			ss.setf(ios::showpos);
			ss.setf(ios_base::fixed, ios_base::floatfield);
			ss.precision(2);

			ss << "[Drag " << i << " " << dragModes[i]->GetDragVector() << "] ";
			renderTexts.push_back(ss.str());
		}
	}

	if (RenderCameraPosition)
	{
		stringstream ss;
		ss.setf(ios::showpos);
		ss.setf(ios_base::fixed, ios_base::floatfield);
		ss.precision(2);

		ss << "[Camera " << GetCameraVector() << "] ";
		renderTexts.push_back(ss.str());
	}
}

void GlutApplication::Reshape()
{
	this->VimridApplication::Reshape();
}

void GlutApplication::Exit()
{
	this->VimridApplication::Exit();

	if (mFullScreenActive)
	{
		leaveFullScreen();
	}

	// GLUT loop cannot be stopped, so force quit application.
	exit(0);
}

void GlutApplication::glutHandleDisplay()
{
	UpdateAndRender();

	// See method comments for explanation.
	clearGlutKeyModifiers();
}

void GlutApplication::glutHandleReshape(int width, int height)
{
	SetWindowSize(width, height);
	Reshape();
}

/* This must be called from an input callback, otherwise GLUT complains.
 */
void GlutApplication::updateGlutKeyModifiers()
{
	GetGlutInputManager()->UpdateKeyModifiers(glutGetModifiers());
}

/* This should be called after each update and render, because the value
 * is only set on GLUT input callbacks (there is no way of un-setting
 * it once the key has been released), so we must always assume that no
 * modifier is valid when there is no keyboard/mouse input. */
void GlutApplication::clearGlutKeyModifiers()
{
	GetGlutInputManager()->UpdateKeyModifiers(0);
}

void GlutApplication::glutHandleKeyboard(unsigned char key, int x, int y)
{
	GetGlutInputManager()->UpdateCharKeyDown(key);
	updateGlutKeyModifiers();
}

void GlutApplication::glutHandleKeyboardUp(unsigned char key, int x, int y)
{
	GetGlutInputManager()->UpdateCharKeyUp(key);
	updateGlutKeyModifiers();
}

void GlutApplication::glutHandleSpecial(int key, int x, int y)
{
	GetGlutInputManager()->UpdateSpecialKeyDown(key);
	updateGlutKeyModifiers();
}

void GlutApplication::glutHandleSpecialUp(int key, int x, int y)
{
	GetGlutInputManager()->UpdateSpecialKeyUp(key);
	updateGlutKeyModifiers();
}

void GlutApplication::glutHandleMouse(int button, int state, int x, int y)
{
	GetGlutInputManager()->UpdateMousePosition(x, y);
	GetGlutInputManager()->UpdateMouseButton(button, state);
	//updateGlutKeyModifiers();
}

void GlutApplication::glutHandleMotion(int x, int y)
{
	GetGlutInputManager()->UpdateMousePosition(x, y);
	updateGlutKeyModifiers();
}

void GlutApplication::glutHandlePassiveMotion(int x, int y)
{
	/* Passive motion occurs when no key is pressed, so a call
	 * to the glutGetModifiers() function is invalid here.
	 */
	GetGlutInputManager()->UpdateMousePosition(x, y);
}

void GlutApplication::glutCallbackDisplay()
{
	GetGlutInstance()->glutHandleDisplay();
}

void GlutApplication::glutCallbackReshape(int width, int height)
{
	GetGlutInstance()->glutHandleReshape(width, height);
}

void GlutApplication::glutCallbackKeyboard(unsigned char key, int x, int y)
{
	GetGlutInstance()->glutHandleKeyboard(key, x, y);
}

void GlutApplication::glutCallbackKeyboardUp(unsigned char key, int x, int y)
{
	GetGlutInstance()->glutHandleKeyboardUp(key, x, y);
}

void GlutApplication::glutCallbackSpecial(int key, int x, int y)
{
	GetGlutInstance()->glutHandleSpecial(key, x, y);
}

void GlutApplication::glutCallbackSpecialUp(int key, int x, int y)
{
	GetGlutInstance()->glutHandleSpecialUp(key, x, y);
}

void GlutApplication::glutCallbackMouse(int button, int state, int x, int y)
{
	GetGlutInstance()->glutHandleMouse(button, state, x, y);
}

void GlutApplication::glutCallbackMotion(int x, int y)
{
	GetGlutInstance()->glutHandleMotion(x, y);
}

void GlutApplication::glutCallbackPassiveMotion(int x, int y)
{
	GetGlutInstance()->glutHandlePassiveMotion(x, y);
}

}
}
