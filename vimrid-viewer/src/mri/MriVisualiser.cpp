/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 20-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "MriVisualiser.h"
#include "../VimridApplication.h"
#include <vector>

using namespace std;
using namespace vimrid::sprites;
using namespace vimrid::textures;
using namespace vimrid::dicom;
using namespace vimrid::imaging;
using namespace vimrid::input;

namespace vimrid
{
namespace mri
{

MriVisualiser::MriVisualiser(
	SpriteManager &spriteManager,
	VTextureManager &textureManager,
	DragController &dragController,
	VSizeF &windowSize)
	:
	mSpriteManager(spriteManager),
	mTextureManager(textureManager),
	mScrollController(dragController, windowSize)
{
}

MriVisualiser::~MriVisualiser()
{
	deleteData();
}

void MriVisualiser::deleteData()
{
	DEBUG("Deleting " << mTextures.size() << " MRI visualiser textures...");
	for (size_t i = 0; i < mTextures.size(); i++)
	{
		DEBUG("Deleting texture: " << mTextures[i]->GetTextureId());
		delete mTextures[i];
	}

	/* Delete images that have been created within this class;
	 * this will not delete images where delete repsonsibility
	 * lies with a Texture object.
	 */
	for (size_t i = 0; i < mImages.size(); i++)
	{
		delete mImages[i];
	}
}

void MriVisualiser::Clear()
{
	DEBUG("Clearing MRI visualiser...");

	mTextures.clear();
	mImages.clear();
	removeSliceTextures();
	removeSliceSprites();
	mMriSet.Clear();
	//mScrollController.Reset();
	deleteData();

	DEBUG("MRI visualiser cleared.");
	DEBUG("Textures in vector: " << mTextures.size());
}

void MriVisualiser::removeSliceSprites()
{
	vector<MriSlice*> &slices = mMriSet.GetSlices();
	for (size_t i = 0; i < slices.size(); i++)
	{
		mSpriteManager.Remove(slices[i]);
	}
}

void MriVisualiser::removeSliceTextures()
{
	for (size_t i = 0; i < mTextures.size(); i++)
	{
		mTextureManager.Remove(mTextures[i]);
	}
}

const VimridUtility &MriVisualiser::GetUtility() const
{
	return VimridUtility::Get();
}

void MriVisualiser::AddDicomImages(std::vector<DicomImage*> dicomImages)
{
	for (VUInt32 i = 0; i < dicomImages.size(); i++)
	{
		/* Create a new image matrix and add it to the images vector so that
		 * it can be deleted upon destruction. */
		DicomImage &dicomImage = *dicomImages[i];
		ImageMatrix *image = dicomImage.ToImageMatrix();
		mImages.push_back(image);

		/* Normalise the image so that the pixels are compatible with OpenGL;
		 * converting the bit length doesn't seem to be reliable across the
		 * board; sometimes the values fall just outside the valid bit length
		 * which is either a bug or by design. */
		image->Normalise(0, 255);

		AddImageAsSlice(*image);
	}
}

void MriVisualiser::AddImageAsSlice(vimrid::imaging::ImageMatrix &image)
{
	/* Create a new texture and add it to the textures vector so that
	 * it can be deleted upon destruction. */
	LuminanceTexture *texture = new LuminanceTexture(image);
	mTextures.push_back(texture);

	// Add to to the slice list so the texture can be bound and rendered.
	AddTextureAsSlice(texture);
}

void MriVisualiser::AddTextureAsSlice(VTexture *texture)
{
	MriSlice *slice = mMriSet.AddTextureAsSlice(texture);

	/* Add to the texture manager so that it can be created with all
	 * other textures in the system that controls this object. */
	mTextureManager.Add(texture);

	/* Add to sprite manager so that it is rendered on screen with
	 * all of the other sprite objects. */
	mSpriteManager.Add(*slice);
}

void MriVisualiser::Update()
{
	const VFloat sliceSeparation = 0.02;
	const VFloat xOffset = 1.0;

	// Allow the scroll controller to calculate each slice position based on slices count.
	mScrollController.SetItemCount(mMriSet.SliceCount());

	for (VUInt32 i = 0; i < mMriSet.SliceCount(); i++)
	{
		MriSlice &slice = mMriSet[i];
		VPoint3f sliceVector = mScrollController.GetItemPoint(i, GetUtility());

		// Separate slices by a small amount.
		sliceVector.Y += i * sliceSeparation;

		// Start slices to the left so there's room for the scrolled slice.
		sliceVector.X += xOffset;

		slice.SetPosition(sliceVector);
	}
}

}
}
