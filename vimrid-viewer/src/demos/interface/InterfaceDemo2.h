/*
 * InterfaceDemo1.h
 *
 *  Created on: 02-Apr-2009
 *      Author: nick
 */

#ifndef INTERFACEDEMO2_H_
#define INTERFACEDEMO2_H_

#include "../../glut/GlutApplication.h"
#include "../../VimridMenu.h"
#include <vimrid.h>

namespace vimrid
{
namespace textures
{

class VTexture;

}
namespace demos
{
namespace interface
{

class InterfaceDemo2: public vimrid::glut::GlutApplication
{
public:
	InterfaceDemo2(VimridSettings &settings);
	virtual ~InterfaceDemo2();
protected:
	void Init();
	void Update();
	void Render();
	void Reshape();
	virtual void OnControlSelectRelease(vimrid::ui::Control &child);
private:
	vimrid::dicom::DicomTcpClient mDicomClient;
	vimrid::dicom::AsyncDicomResult *mDicomResult;
	vimrid::ui::Control *mDicomLoadingScreen;
	void handleDownload(void *argPtr);
	static void *_handleDownload(void *argPtr);
	void setupMenu();
};

}
}
}

#endif // INTERFACEDEMO2_H_
