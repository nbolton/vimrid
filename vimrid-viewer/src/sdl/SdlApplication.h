#ifndef SDLAPPLICATION_H_
#define SDLAPPLICATION_H_

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include "../VimridApplication.h"
#include "SdlInputManager.h"

namespace vimrid
{
namespace sdl
{
class SdlApplication : public VimridApplication
{
public:
	SdlApplication(VimridSettings &settings);
	virtual ~SdlApplication();
	void Run();
	static SdlApplication &GetSdlInstance();

protected:
	VPoint2f WindowPoint;
	virtual void Init();
	virtual void Update();
	virtual void Render();
	virtual void Reshape();
	virtual void Exit();
	virtual void FinishRender();
	SdlInputManager *GetSdlInputManager();
	bool IsKeyPressed(int xKey);
	Display &GetXDisplay();
	Window GetXWindow();
	GLXContext GetGlxContext();
	virtual VBool IsFullScreenActive() { return false; }
private:
	void makeWindow();
	void eventLoop();
	void processEvents();
	Display *display;
	Window window;
	GLXContext context;
	bool exitEventLoop;
};
}
}

#endif // SDLAPPLICATION_H_
