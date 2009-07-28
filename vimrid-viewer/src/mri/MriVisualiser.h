/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 20-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef MRIVISUALISER_H_
#define MRIVISUALISER_H_

#include "MriSet.h"
#include "../textures/LuminanceTexture.h"
#include "../textures/VTextureManager.h"
#include "../sprites/SpriteManager.h"
#include "ScrollController.h"
#include "../input/DragController.h"
#include <vector>
#include <vimrid.h>

namespace vimrid
{
namespace mri
{

class MriVisualiser
{
public:
	MriVisualiser(
		vimrid::sprites::SpriteManager &spriteManager,
		vimrid::textures::VTextureManager &textureManager,
		vimrid::input::DragController &dragController,
		VSizeF &windowSize);
	virtual ~MriVisualiser();
	void AddTextureAsSlice(vimrid::textures::VTexture *texture);
	void Update();
	void AddDicomImages(std::vector<vimrid::dicom::DicomImage*> images);
	void AddImageAsSlice(vimrid::imaging::ImageMatrix &image);
	const VimridUtility &GetUtility() const;
	void Clear();
	ScrollController &GetScrollController() { return mScrollController; }
private:
	MriSet mMriSet;
	std::vector<vimrid::textures::VTexture*> mTextures;
	std::vector<vimrid::imaging::ImageMatrix*> mImages;
	vimrid::sprites::SpriteManager &mSpriteManager;
	vimrid::textures::VTextureManager &mTextureManager;
	ScrollController mScrollController;
	void deleteData();
	void removeSliceSprites();
	void removeSliceTextures();
};

}
}

#endif // MRIVISUALISER_H_
