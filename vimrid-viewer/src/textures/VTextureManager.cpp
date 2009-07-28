/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 09-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "VTextureManager.h"
#include "GL/gl.h"
#include <algorithm>

using namespace std;

namespace vimrid
{
namespace textures
{

VTextureManager::VTextureManager() :
	mCreatePending(false),
	textureIds(NULL)
{
}

VTextureManager::~VTextureManager()
{
	delete textureIds;
}

VUInt32 VTextureManager::Count()
{
	return mTextures.size();
}

void VTextureManager::Add(VTexture *texture)
{
	mTextures.push_back(texture);
	mCreatePending = true;
}

void VTextureManager::Remove(VTexture *texture)
{
	FindPtrPredicate<VTexture*> predicate = { texture };

	vector<VTexture*>::iterator it = find_if(
		mTextures.begin(), mTextures.end(), predicate);

	mTextures.erase(it);
}

VBool VTextureManager::IsCreatePending()
{
	return mCreatePending;
}

void VTextureManager::CreateAll()
{
	// If re-binding delete the existing IDs.
	if (textureIds != NULL)
	{
		delete[] textureIds;
	}

	// Recreate array as texture count may have changed.
	textureIds = new GLuint[mTextures.size()];

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(mTextures.size(), textureIds);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// Associated the OpenGL IDs with the actual textures.
	for (VUInt32 i = 0; i < mTextures.size(); i++)
	{
		VTexture &texture = *mTextures[i];

		// Textures can only be created once.
		if (!texture.IsCreated())
		{
			texture.SetTextureId(textureIds[i]);
			texture.Create();
		}
	}

	// All textures created, nothing to do.
	mCreatePending = false;
}

}
}
