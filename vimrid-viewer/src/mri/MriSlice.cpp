/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 20-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "MriSlice.h"

namespace vimrid
{
namespace mri
{

MriSlice::MriSlice(vimrid::textures::VTexture *texture, const VUInt32 &renderIndex)
:
	Sprite(renderIndex),
	mTexture(texture)
{
}

MriSlice::~MriSlice()
{
}

void MriSlice::Update()
{

}

void MriSlice::Render()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	TranslateToPosition();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	if (mTexture->IsCreated())
	{
		mTexture->Bind();
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, 0.0, -1.0); // top left
		glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, 0.0, 1.0); // bottom left
		glTexCoord2f(1.0, 1.0); glVertex3f(+1.0, 0.0, 1.0); // bottom right
		glTexCoord2f(1.0, 0.0); glVertex3f(+1.0, 0.0, -1.0); // top right
		glEnd();
		mTexture->Unbind();
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

}
}
