/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 17-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "LuminanceTexture.h"

using namespace vimrid::imaging;

namespace vimrid
{
namespace textures
{

LuminanceTexture::LuminanceTexture(ImageMatrix &image)
	: VTexture(image, GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE)
{
}

LuminanceTexture::~LuminanceTexture()
{
}

}
}
