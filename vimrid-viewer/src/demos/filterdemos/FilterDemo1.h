#ifndef FILTERDEMO1_H_
#define FILTERDEMO1_H_

#include "../../glut/GlutApplication.h"

namespace vimrid
{
namespace demos
{
namespace filterdemos
{

class FilterDemo1 : public vimrid::glut::GlutApplication
{
public:
	FilterDemo1(VimridSettings &settings);
	virtual ~FilterDemo1();
	void Init();
	void Update();
	void Render();
	void Reshape();
private:
	GLuint textureIds[2];
	vimrid::dicom::DicomImage *loadDicomImage(
		std::string fileName,
		std::string hostName,
		int port);
	void createTexture(
		GLuint *textureId,
		const vimrid::dicom::DicomImage &dicomImage);
	void createTexture(
		GLuint *textureId,
		const vimrid::imaging::ImageMatrix &image);
	void createTexture(
		GLuint *textureId,
		GLuint width,
		GLuint height,
		const GLubyte *pixels);
	vimrid::imaging::ImageMatrix *convoluteImage(
		const vimrid::imaging::ImageMatrix &image,
		const vimrid::imaging::ImageMatrix &filter);
	vimrid::imaging::ImageMatrix *combineImages(
		const vimrid::imaging::ImageMatrix &image1,
		const vimrid::imaging::ImageMatrix &image2);
};

}
}
}


#endif // FILTERDEMO1_H_
