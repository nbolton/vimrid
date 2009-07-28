/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 09-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef VTEXTUREMANAGER_H_
#define VTEXTUREMANAGER_H_

#include <vector>
#include <vimrid.h>
#include "VTexture.h"
#include <GL/gl.h>

namespace vimrid
{
namespace textures
{

class VTextureManager
{
public:
	VTextureManager();
	virtual ~VTextureManager();
	void Add(VTexture *texture);
	void CreateAll();
	VBool IsCreatePending();
	VUInt32 Count();
	void Remove(VTexture *texture);
private:
	VBool mCreatePending;
	std::vector<VTexture*> mTextures;
	GLuint *textureIds;
};

}

}

#endif // VTEXTUREMANAGER_H_
