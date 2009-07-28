/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 24-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef SAMPLETEXTURE_H_
#define SAMPLETEXTURE_H_

#include "VTexture.h"

namespace vimrid
{
namespace imaging
{

class ImageMatrix;

}
namespace textures
{

class SampleTexture : public VTexture
{
public:
	SampleTexture();
	virtual ~SampleTexture();
private:
	vimrid::imaging::ImageMatrix *mImage;
};

}
}

#endif // SAMPLETEXTURE_H_
