#ifndef MRIDEMO1_H_
#define MRIDEMO1_H_

#include <vimrid.h>
#include "../../glut/GlutApplication.h"
#include "../../textures/VTextureManager.h"
#include "../../mri/MriVisualiser.h"

namespace vimrid
{
namespace demos
{
namespace mri
{

class MriDemo1 : public vimrid::glut::GlutApplication
{
public:
	MriDemo1(VimridSettings &settings);
	virtual ~MriDemo1();
	void Init();
	void Update();
	void Render();
	void Reshape();
private:
	vimrid::dicom::DicomTcpClient mDicomClient;
	vimrid::dicom::AsyncDicomResult *mDicomResult;
	void handleDownload(void *argPtr);
	static void *_handleDownload(void *argPtr);
};

}
}
}

#endif // MRIDEMO1_H_
