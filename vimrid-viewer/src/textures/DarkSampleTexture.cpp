/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 24-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "DarkSampleTexture.h"
#include <vimrid.h>

using namespace vimrid::imaging;

namespace vimrid
{
namespace textures
{

DarkSampleTexture::DarkSampleTexture()
{
	int i, j;
	VUByte pixel;
	ImageMatrix *sourceImage = new ImageMatrix(64, 64, 4);
	for (i = 0; i < 64; i++)
	{
		for (j = 0; j < 64; j++)
		{
			pixel = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 100;

			sourceImage->Get(i, j, 0) = pixel;
			sourceImage->Get(i, j, 1) = pixel;
			sourceImage->Get(i, j, 2) = pixel;
			sourceImage->Get(i, j, 3) = 255;
		}
	}
	mImage = sourceImage;
	SetImage(*sourceImage);
}

DarkSampleTexture::~DarkSampleTexture()
{
	delete mImage;
}

}
}
