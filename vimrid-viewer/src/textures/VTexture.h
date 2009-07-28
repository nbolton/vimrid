/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 09-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef VTEXTURE_H_
#define VTEXTURE_H_

#include <vimrid.h>
#include "GL/gl.h"

namespace vimrid
{
namespace imaging
{

class ImageMatrix;

}
namespace textures
{

class VTexture
{
public:
	VTexture();
	VTexture(vimrid::imaging::ImageMatrix &image);
	VTexture(vimrid::imaging::ImageMatrix &image, GLenum format, GLint internalFormat, GLenum type);
	virtual ~VTexture();
	void SetTextureId(GLuint textureId) { mTextureId = textureId; }
	GLuint GetTextureId() const { return mTextureId; }
	void Create();
	void Bind();
	void Unbind();
	void SetFormat(GLenum value) { mFormat = value; }
	void SetInternalFormat(GLint value) { mInternalFormat = value; }
	VBool IsCreated();
	//VBool IsCreatedSafe();
protected:
	void SetImage(vimrid::imaging::ImageMatrix &image) { mImage = &image; }
private:
	VBool mIsCreated;
	VUInt64 mCreateTime;
	GLuint mTextureId;
	vimrid::imaging::ImageMatrix *mImage;
	GLenum mFormat;
	GLint mInternalFormat;
	GLenum mType;
};

}
}

#endif // VTEXTURE_H_
