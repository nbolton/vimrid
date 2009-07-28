/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 17-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef LUMINANCETEXTURE_H_
#define LUMINANCETEXTURE_H_

#include "VTexture.h"
#include <vimrid.h>

namespace vimrid
{
namespace textures
{

class LuminanceTexture: public vimrid::textures::VTexture
{
public:
	LuminanceTexture(vimrid::imaging::ImageMatrix &image);
	virtual ~LuminanceTexture();
};

}
}

#endif // LUMINANCETEXTURE_H_
