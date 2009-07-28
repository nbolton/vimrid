/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 07-May-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef VIMRIDVIEWER_H_
#define VIMRIDVIEWER_H_

#include "../glut/GlutApplication.h"
#include "../stereo/StereoApplication.h"
#include "../demos/cubedemos/TestCube.h"
#include <vimrid.h>
#include <pthread.h>

namespace vimrid
{
class VimridSettings;
class VimridMenu;
namespace ui
{
class Control;
class Button;
}
namespace viewer
{
enum VimridViewerFilterMode
{
	VV_FM_None,
	VV_FM_SobelXNormal,
	VV_FM_SobelYNormal,
	VV_FM_SobelXYNormal,
	VV_FM_SobelXYScoped
};
class VimridViewer :
	public vimrid::glut::GlutApplication,
	public vimrid::stereo::StereoApplication
{
public:
	VimridViewer(VimridSettings &settings);
	virtual ~VimridViewer();
protected:
	void Init();
	void Update();
	void Render();
	void Reshape();
	virtual void RenderDelegate();
	virtual VFloat GetAspectRatio() const;
	virtual void OnControlSelectRelease(vimrid::ui::Control &child);
	virtual void OnControlShown(vimrid::ui::Control &control);
	virtual void OnControlHidden(vimrid::ui::Control &control);
	virtual VPoint3f GetCameraVector();
private:
	vimrid::dicom::DicomTcpClient mDicomClient;
	vimrid::dicom::AsyncDicomResult *mDicomResult;
	vimrid::VimridMenu *mDicomLoadingScreen;
	vimrid::VimridMenu *mFilterLoadingScreen;
	vimrid::VimridMenu *mMainMenu;
	pthread_t mProcessorThread;
	pthread_t mFilterRestoreThread;
	vimrid::ui::Button *mFilterMenuButton;
	vimrid::ui::Button *mSobelXNormalButton;
	vimrid::ui::Button *mSobelYNormalButton;
	vimrid::ui::Button *mSobelXYNormalButton;
	vimrid::ui::Button *mSobelXYScopedButton;
	vimrid::ui::Button *mFilterRestoreButton;
	VPoint3f mCameraPushbackSaved;
	VBool mCameraPushedBack;
	VPoint3f mCameraAnimationTarget;
	VPoint3f mCameraAnimationBuffer;
	vimrid::ui::Button *mToggleStereoButton;
	vimrid::ui::Button *mToggleTrackdButton;
	vimrid::ui::Button *mToggleAlphaButton;
	vimrid::ui::Button *mExitVimridButton;
	VimridViewerFilterMode mFilterMode;
	VBool mDicomLoadCancel;
	vimrid::ui::Button *mFilterLoadingScreenCancelButton;
	VBool mAlphaEnabled;
	/*VPoint3f mCameraCouteractNeg;
	VPoint3f mCameraCouteractPos;*/
	VPoint3f mCameraCouteract;
	VPoint3f mLastCameraVector;
	VPoint3f mCameraDragDelta;
	VPoint3f mLastCameraDragVector;
	VBool mMenuAutoShown;
	void handleDownload(void *argPtr);
	static void *_handleDownload(void *argPtr);
	void setupInterface();
	static void *_processImages(void *argPtr);
	void processImages();
	static void *_filterRestore(void *argPtr);
	void filterRestore();
	void defaultCamera();
	VPoint3f getCameraDragVector() const;
};
}
}

#endif // VIMRIDVIEWER_H_
