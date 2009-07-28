#ifndef GLUTAPPLICATION_H_
#define GLUTAPPLICATION_H_

#include <string>
#include <iostream>
#include <GL/glut.h>
#include "../VimridApplication.h"
#include "GlutInputManager.h"
#include <sstream>

#define ADD_STATUS_LINE(stream) \
{ \
	StatusLineBuffer.str(""); \
	StatusLineBuffer << stream; \
	AddStatusLine(StatusLineBuffer.str()); \
}

namespace vimrid
{
namespace glut
{

class GlutApplication : public VimridApplication
{
public:
	GlutApplication(VimridSettings &settings);
	virtual ~GlutApplication();
	void Run();
	static GlutApplication *GetGlutInstance();
	GlutInputManager *GetGlutInputManager();
protected:
	float DepthPerception;
	Point3f CameraPoint;
	Point3f CameraTrim;
	Point3f FeatureRotation;
	//VUInt32 DisplayMode;
	VBool EnableCursor;
	VBool RenderFps;
	VBool RenderCameraPosition;
	VBool RenderDragInfo;
	VBool RenderMouseInfo;
	VBool RenderStatusTextOnFinish;
	VBool EnableRenderStatusText;
	VBool RenderTrackdInfo;
	std::stringstream StatusLineBuffer;
	void RenderStringOrtho2D(Point2f point, void* font, std::string string);
	virtual void Init();
	virtual void Update();
	virtual void Render();
	virtual void Reshape();
	virtual void Exit();
	virtual void FinishRender();
	VBool IsSpecialKeyPressed(int glutSpecialKey);
	VBool IsCharKeyPressed(char charKey);
	VBool IsModiferKeyPressed(int glutModifierKey);
	virtual VPoint3f GetCameraVector() { return CameraPoint + CameraTrim; }
	void RenderStatusText();
	void AddStatusLine(const std::string &line) { renderTexts.push_back(line); }
	VUInt32 GetDisplayMode() const;
	virtual VBool IsFullScreenActive() { return mFullScreenActive; }
private:
	std::vector<std::string> renderTexts;
	Point3f mCameraPosition;
	VBool mFullScreenActive;
	void enterFullScreen();
	void leaveFullScreen();
	void setupStatusLines();
	static void *glutLoopStart(void *pArg);
	void startGlut();
	void updateGlutKeyModifiers();
	void clearGlutKeyModifiers();
	void glutHandleDisplay();
	void glutHandleReshape(int width, int height);
	void glutHandleKeyboard(unsigned char key, int x, int y);
	void glutHandleKeyboardUp(unsigned char key, int x, int y);
	void glutHandleMotion(int x, int y);
	void glutHandlePassiveMotion(int x, int y);
	void glutHandleMouse(int button, int state, int x, int y);
	void glutHandleSpecial(int key, int x, int y);
	void glutHandleSpecialUp(int key, int x, int y);
	static void glutCallbackDisplay();
	static void glutCallbackReshape(int width, int height);
	static void glutCallbackKeyboard(unsigned char key, int x, int y);
	static void glutCallbackKeyboardUp(unsigned char key, int x, int y);
	static void glutCallbackMotion(int x, int y);
	static void glutCallbackPassiveMotion(int x, int y);
	static void glutCallbackMouse(int button, int state, int x, int y);
	static void glutCallbackSpecial(int key, int x, int y);
	static void glutCallbackSpecialUp(int key, int x, int y);
};

}
}

#endif // GLUTAPPLICATION_H_
