/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 20-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef MRISLICE_H_
#define MRISLICE_H_

#include "../textures/VTexture.h"
#include "../sprites/Sprite.h"

namespace vimrid
{
namespace mri
{

class MriSlice : public vimrid::sprites::Sprite
{
public:
	MriSlice(vimrid::textures::VTexture *texture, const VUInt32 &renderIndex);
	virtual ~MriSlice();
	virtual void Update();
	virtual void Render();
private:
	vimrid::textures::VTexture *mTexture;
};

}
}

#endif // MRISLICE_H_
