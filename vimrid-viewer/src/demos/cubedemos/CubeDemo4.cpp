/* 21/02/09 - Just tried this and stereo doesn't work at all.
 *
 * Attempted to implement reald tutorial...
 * http://74.125.77.132/search?q=cache:MtrDJTxS5B4J:www.lsqr.com/scientific/downloads/hpsdk.doc+%22Writing+Stereoscopic+Software+for+StereoGraphics%22&hl=en&ct=clnk&cd=1&gl=uk
 * with little such luck. Will try further methods in next demo...
 *
 * In this demo I will work to produce the gluFrustum function to prouce accurate
 * stereo visualisation, and I will try to improve the feature object rotation
 * caused by wand movement.
 *
 * Also, will try out new trackd 'device' system which implements the trackd controllers.
 *
 * Tutorials...
 * 1. http://local.wasp.uwa.edu.au/~pbourke/miscellaneous/stereorender/ - 1# Google result.
 * 2. http://www.orthostereo.com/geometryopengl.html - Code looks a bit more in-tact than #1?
 * 3. http://reald-corporate.com/scientific/downloads/pcsdk.htm - Right from the horses mouth!
 *
 * StereoGraphics developer support can also be reached at support@crystaleye.com, or 415-459-4500.
 */

#include "CubeDemo4.h"

using namespace std;
using namespace vimrid::input;

namespace vimrid
{
namespace demos
{
namespace cubedemos
{

CubeDemo4::CubeDemo4(VimridSettings &settings)
	: GlutApplication(settings)
{
	Title = "ViMRID Test Cube - v4 - 15/02/09";
	SetWindowSize(800, 800);

	stereoOffsetAdjust = 1.0;
	stereoBalanceAdjust = 1.0;

	wandDragActive = false;

	if (GetInputManager()->IsTrackdSupported())
	{
		GetInputManager()->InitTrackd(TD_TRACKER_KEY);
		glasses = &GetInputManager()->AddTrackdDevice(TD_GLASSES_ID);
		wand = &GetInputManager()->AddTrackdDevice(TD_WAND_ID, TD_WAND_CONTROLLER_KEY);
	}

	mouse = &GetInputManager()->GetMouseInfo();
}

void CubeDemo4::Init()
{
	this->GlutApplication::Init();

	createTestCube();
}

void CubeDemo4::Update()
{
	this->GlutApplication::Update();

	if (IsSpecialKeyPressed(GLUT_KEY_UP))
	{
		CameraTrim.Z += .5;
	}

	if (IsSpecialKeyPressed(GLUT_KEY_DOWN))
	{
		CameraTrim.Z -= .5;
	}

	if (IsSpecialKeyPressed(GLUT_KEY_PAGE_UP))
	{
		stereoOffsetAdjust += .01;
	}

	if (IsSpecialKeyPressed(GLUT_KEY_PAGE_DOWN))
	{
		stereoOffsetAdjust -= .01;
	}

	if (IsSpecialKeyPressed(GLUT_KEY_HOME))
	{
		stereoBalanceAdjust += .01;
	}

	if (IsSpecialKeyPressed(GLUT_KEY_END))
	{
		stereoBalanceAdjust -= .01;
	}

	updateCamera();
}

void CubeDemo4::Render()
{
	this->GlutApplication::Render();

	/* There are apparently(1) two particular ways of renderng stereo, one is known as
	 * toe-in and the other, off-axis. The latter is considered correct and according
	 * to the link below, "it requires a non symmetric camera frustum," which is
	 * supported by OpenGL. Also, "A good ballpark separation of the cameras is 1/20
	 * of the distance to the projection plane".
	 * 1. http://local.wasp.uwa.edu.au/~pbourke/miscellaneous/stereorender/ */

	drawCube(GL_BACK_LEFT);
	drawCube(GL_BACK_RIGHT);

	/*VPoint3f* rbsPoint = new VPoint3f();
	rbsPoint->X = -10;
	rbsPoint->Y = -10;
	RenderBitmapString(rbsPoint, GLUT_BITMAP_HELVETICA_18, (char*)"Hello world!");*/

	FinishRender();
}

void CubeDemo4::drawCube(GLenum buffer)
{
	glDrawBuffer(buffer);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// parallax of about 3% reccomended by reald / StereoGraphics corp.
	double reccomendedParallax = .035;

	// should these be mathmatically calculated or are they arbitrary?
	double nearClipDistance = 5; // 5 because i saw this in some example code, should it be 5?
	double zeroParallaxDistance = 200;
	double farClipDistance = 400;

	double aspectRatio = GetWindowSize().Width / GetWindowSize().Height;

	// correct to reflect values of aspect ratio so simply?
	double frustumLeft = -aspectRatio;
	double frustumRight = +aspectRatio;
	double frustumTop = -aspectRatio;
	double frustumBottom = +aspectRatio;

	/* also known as Xrange, this equals the horizontal range of the scene along the
	 * desired plane of zero parallax. One way to derive Xrange is to get the difference
	 * between glFrustum()’s first two arguments, which represent the projection frustum’s
	 * horizontal range at the near clipping plane, and multiply that difference by the
	 * ratio of the distance to the desired plane of zero parallax to the near clipping
	 * plane distance. Another way to calculate Xrange would be to multiply the distance
	 * to the desired plane of zero parallax by two times the tangent of half the horizontal
	 * field of view angle.
	 *
	 * xrange = (left + right) * (zero / near)
	 */
	double horizontalFrustumRange = (frustumLeft + frustumRight) * (zeroParallaxDistance / nearClipDistance);

	// which side of the center is the eye on?
	int eyeSide = (buffer == GL_BACK_LEFT) ? -1 : +1;

	// used by both glTranslated and glFrustum
	double stereoCameraOffset = horizontalFrustumRange * reccomendedParallax * eyeSide;

	// not sure why this is neccecary?
	double nOverD = nearClipDistance * zeroParallaxDistance;

	// to be used by glFrustum only
	double frustumAsymetry = -stereoCameraOffset * stereoBalanceAdjust * nOverD;

	/* we need to use a bit of negative (where object appears infront of screen) and positive,
	 * because unfortunately all positive tends to be uncomfortable to look at(1).
	 * 1. http://reald-corporate.com/scientific/downloads/pcsdk.htm
	 *
	 * Use a separation value of 3% of the screen (viewport) width. And, the field of view
	 * should be at a 50 degree angle or more. */
	glFrustum(frustumLeft + frustumAsymetry,
		frustumRight + frustumAsymetry,
		frustumTop,
		frustumBottom,
		nearClipDistance,
		farClipDistance);

	// many demos seem to call this, and others do not, why? shouln't only frustum shift be needed???
	// ... will try and use glLookAt instead
	// according to reald, X translation must be used along with glFrustum (must be opposite of stereo camera offset
	glTranslated(((stereoCameraOffset * stereoOffsetAdjust) * -1), 0, 0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslated(0, 0, -50);

	glEnable(GL_CULL_FACE);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	// some demos seem to move the camera position (but shouldnt both cameras be looking straight ahead?)
	gluLookAt(CameraPoint.X, CameraPoint.Y, CameraPoint.Z, 0, 0, 0, 0, 1, 0);

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
	glPopMatrix();
}

void CubeDemo4::Resized()
{
	glViewport(0, 0, (int)GetWindowSize().Width, (int)GetWindowSize().Height);
	//aspectRatio = (GLfloat)GetWindowSize().Height / (GLfloat)GetWindowSize().Width;

	/*GLfloat w = fixationPoint * (1.0 / 5.0);
	left = -5.0 * ((w - 0.5 * eyeSeparation) / fixationPoint);
	right = 5.0 * ((w + 0.5 * eyeSeparation) / fixationPoint);*/
}

/* This tutorial is Copyright © 2001 Ben Woodhouse
 * http://www.gamedev.net/reference/articles/article1681.asp
 */
void CubeDemo4::createTestCube()
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

void CubeDemo4::updateCamera()
{
	if (GetInputManager()->IsTrackdSupported())
	{
		// Move the camera to where the glasses are.
		CameraPoint.X = CameraTrim.X + (glasses->GetSensor().GetPosition().X) * 20;
		CameraPoint.Y = CameraTrim.Y + 100 + ((glasses->GetSensor().GetPosition().Z) * 30);
		CameraPoint.Z = CameraTrim.Z + 100 + (100 - ((glasses->GetSensor().GetPosition().Y) * 10));

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
			/*FeatureRotation.Y += wand->Sensor.GetPoint().Z * 10;
			FeatureRotation.Z += (wand->Sensor.GetPoint().Y * -1) * 10;*/

			//lastWandPoint.X = wand->Sensor.GetPoint().X;
		}
		else
		{
			wandDragActive = false;
		}
	}
	else
	{
		// When mouse moves up and down, move toward/away from screen.
		CameraPoint.X = CameraTrim.X + (((GetWindowSize().Width / 2) * -1) + mouse->GetPosition().X) * 0.15;
		CameraPoint.Y = CameraTrim.Y + (((GetWindowSize().Height / 2) * -1) + mouse->GetPosition().Y) * -0.2;
		CameraPoint.Z = CameraTrim.Z + 50;// + (mouse.GetPoint().Y) * 0.2;
	}

	/*cout << "Camera "
		<< " X: " << CameraPoint.X
		<< " Y: " << CameraPoint.Y
		<< " Z: " << CameraPoint.Z << endl;*/

	/*cout << "Rotation "
		<< " X: " << FeatureRotation.X
		<< " Y: " << FeatureRotation.Y
		<< " Z: " << FeatureRotation.Z << endl;*/

}

}
}
}
