#ifndef VIMRIDAPPLICATION_H_
#define VIMRIDAPPLICATION_H_

#include <vimrid.h>
#include "input/InputManager.h"
#include "VimridSettings.h"
#include <string>
#include <sys/time.h>
#include "sprites/SpriteManager.h"
#include "textures/VTextureManager.h"
#include "mri/MriVisualiser.h"
#include "input/DragController.h"
#include "VimridUtility.h"
#include <vector>
#include "ui/Container.h"

namespace vimrid
{
namespace ui
{
class Control;
}
class VimridMenu;
class VimridUtility;
class VimridApplication
{
friend class VimridMenu;
friend class VimridUtility;
public:
	int ErrorCode;
	std::string Title;
	VimridApplication(VimridSettings &settings, vimrid::input::InputManager *inputManager);
	virtual ~VimridApplication();
	virtual void Run();
	static VimridApplication *GetInstance();
protected:
	double FrameStartTime;
	bool PrintFps;
	VFloat FramesPerSecond;
	virtual void Init();
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void Reshape();
	virtual void FinishRender();
	virtual void Exit();
	VimridSettings &GetSettings() { return mSettings; }
	void UpdateAndRender();
	void UpdateSprites();
	void RenderSprites();
	vimrid::sprites::SpriteManager &GetSpriteManager() { return mSpriteManager; }
	vimrid::textures::VTextureManager &GetTextureManager() { return mTextureManager; }
	vimrid::input::InputManager *GetInputManager() { return mInputManager; }
	vimrid::mri::MriVisualiser &GetMriVisualiser();
	const vimrid::input::DragController &GetDragController() const;
	const VSizeF &GetWindowSize() { return mWindowSize; }
	void SetWindowSize(const VUInt32 &width, const VUInt32 &height);
	virtual void OnControlShown(vimrid::ui::Control &control);
	virtual void OnControlHidden(vimrid::ui::Control &control);
	virtual void OnControlSelectPress(vimrid::ui::Control &control);
	virtual void OnControlSelectRelease(vimrid::ui::Control &control);
	vimrid::ui::Container &GetUiContainer() { return mUiContainer; }
	virtual VFloat GetAspectRatio() const { return mWindowSize.Height / mWindowSize.Width; }
	void EnableStereo() { mEnableStereo = true; }
	void DisableStereo() { mEnableStereo = false; }
	VBool IsStereoEnabled() const { return mEnableStereo; }
	VimridUtility &GetUtility() { return VimridUtility::Get(); }
	virtual VBool IsFullScreenActive() = 0;
private:
	static VimridApplication *_singleton;
	VimridSettings &mSettings; // No polymorphism, so use reference.
	vimrid::input::InputManager *mInputManager; // May be descendant of InputManager.
	vimrid::sprites::SpriteManager mSpriteManager;
	vimrid::textures::VTextureManager mTextureManager;
	vimrid::mri::MriVisualiser *mMriVisualiser;
	vimrid::input::DragController *mDragController;
	VSizeF mWindowSize;
	VimridUtility mUtility;
	vimrid::ui::Container mUiContainer;
	VBool mEnableStereo;
	void updateFrameRate();
	VBool areMenusVisible();
};
}

#endif // VIMRIDAPPLICATION_H_
