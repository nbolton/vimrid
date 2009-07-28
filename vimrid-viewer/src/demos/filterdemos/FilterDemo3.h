#ifndef FILTERDEMO3_H_
#define FILTERDEMO3_H_

#include "../../glut/GlutApplication.h"
#include "../cubedemos/TestCube.h"
#include "../../textures/LuminanceTexture.h"
#include "../../textures/VTextureManager.h"

namespace vimrid
{
namespace demos
{
namespace filterdemos
{

class FilterDemo3: public vimrid::glut::GlutApplication
{
public:
	FilterDemo3(VimridSettings &settings);
	virtual ~FilterDemo3();
	void HandleDownload(void *argPtr);
protected:
	void Init();
	void Update();
	void Render();
	void Reshape();
private:
	VBool mLoading;
	vimrid::dicom::DicomTcpClient mDicomClient;
	vimrid::dicom::AsyncDicomResult *mDicomResult;
	vimrid::textures::VTextureManager mTextureManager;
	vimrid::textures::LuminanceTexture *mSourceTexture;
	vimrid::textures::LuminanceTexture *mSobelDefaultTexture;
	vimrid::textures::LuminanceTexture *mSobelInvertedTexture;
	vimrid::textures::LuminanceTexture *mSobelScopedTexture;
};

void *_handleDownload(void *argPtr);

}
}
}

#endif // FILTERDEMO3_H_
