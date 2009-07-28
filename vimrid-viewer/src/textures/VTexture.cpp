/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 09-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "VTexture.h"
#include "../VimridUtility.h"

using namespace vimrid::imaging;

namespace vimrid
{
namespace textures
{

// Default constructor values.
#define _VTexture_mIsCreated false
#define _VTexture_mTextureId 0
#define _VTexture_mFormat GL_RGBA
#define _VTexture_mInternalFormat GL_RGBA
#define _VTexture_mType GL_UNSIGNED_BYTE

VTexture::VTexture() :
	mIsCreated(_VTexture_mIsCreated),
	mTextureId(_VTexture_mTextureId),
	mImage(NULL),
	mFormat(_VTexture_mFormat),
	mInternalFormat(_VTexture_mInternalFormat),
	mType(_VTexture_mType)
{
}

VTexture::VTexture(ImageMatrix &image) :
	mIsCreated(_VTexture_mIsCreated),
	mTextureId(_VTexture_mTextureId),
	mImage(&image),
	mFormat(_VTexture_mFormat),
	mInternalFormat(_VTexture_mInternalFormat),
	mType(_VTexture_mType)
{
}

VTexture::VTexture(ImageMatrix &image, GLenum format, GLint internalFormat, GLenum type) :
	mIsCreated(_VTexture_mIsCreated),
	mTextureId(_VTexture_mTextureId),
	mImage(&image),
	mFormat(format),
	mInternalFormat(internalFormat),
	mType(type)
{
}

VTexture::~VTexture()
{
}

VBool VTexture::IsCreated()
{
	return mIsCreated;
}

/* Enforce a 1 second wait before texture can be used, which
 * gives OpenGL time to copy from standard memory to video memory. */
/*VBool VTexture::IsCreatedSafe()
{
	const VUInt32 loadWait = 1000;
	VUInt64 currentTime = VimridUtility::Get().GetTimeMilliseconds();
	VUInt32 elapsedTime = (currentTime - mCreateTime);
	return mIsCreated && (elapsedTime > loadWait);
}*/

void VTexture::Bind()
{
	// Sets this texture as the active texture.
	glBindTexture(GL_TEXTURE_2D, mTextureId);
}

void VTexture::Unbind()
{
	// Clear the current texture.
	glBindTexture(GL_TEXTURE_2D, 0);
}

void VTexture::Create()
{
	if (mIsCreated)
	{
		// Don't allow multiple creation of the same texture.
		THROW_VIMRID_EX("Texture has already been created.");
	}

	// Tell OpenGL which texture we want to create.
	Bind();

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	VUByte *pixels = mImage->CopyPixelsToUByte();

	glTexImage2D(
		GL_TEXTURE_2D, 0, mInternalFormat,
		mImage->GetWidth(), mImage->GetHeight(),
		0, mFormat, mType, pixels);

	// Safe to delete because glTexImage2D copies pixels.
	delete[] pixels;

	// Ensure the texture isn't still bound, as to not cause confusion.
	Unbind();

	// Let implementor know this is now good to use.
	mIsCreated = true;
	mCreateTime = VimridUtility::Get().GetTimeMilliseconds();
}

}
}
