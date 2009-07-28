/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 24-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef DARKSAMPLETEXTURE_H_
#define DARKSAMPLETEXTURE_H_

#include "VTexture.h"

namespace vimrid
{
namespace imaging
{

class ImageMatrix;

}
namespace textures
{

class DarkSampleTexture : public VTexture
{
public:
	DarkSampleTexture();
	virtual ~DarkSampleTexture();
private:
	vimrid::imaging::ImageMatrix *mImage;
};

}
}

#endif // DARKSAMPLETEXTURE_H_
