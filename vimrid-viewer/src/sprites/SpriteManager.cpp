/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 20-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "SpriteManager.h"
#include <algorithm>

using namespace std;

namespace vimrid
{
namespace sprites
{

SpriteManager::SpriteManager()
{
}

SpriteManager::~SpriteManager()
{
}

bool SpriteManager::sortSpriteByRenderIndex(Sprite *a, Sprite *b)
{
	return a->GetRenderIndex() < b->GetRenderIndex();
}

void SpriteManager::Add(Sprite &sprite)
{
	mSprites.push_back(&sprite);
	DEBUG("Added sprite with render index: " << sprite.GetRenderIndex());

	// Sort the sprites in the order they should be rendered.
	sort(mSprites.begin(), mSprites.end(), sortSpriteByRenderIndex);
	DEBUG("Sorted " << mSprites.size() << " sprites by render index.");
}

void SpriteManager::Remove(Sprite *sprite)
{
	FindPtrPredicate<Sprite*> predicate = { sprite };

	vector<Sprite*>::iterator it = find_if(
		mSprites.begin(), mSprites.end(), predicate);

	mSprites.erase(it);
}

void SpriteManager::Update()
{
	for (VUInt32 i = 0; i < mSprites.size(); i++)
	{
		mSprites[i]->TriggerUpdate();
	}
}

void SpriteManager::Render()
{
	for (VUInt32 i = 0; i < mSprites.size(); i++)
	{
		if (mSprites[i]->IsVisible())
		{
			mSprites[i]->TriggerRender();
		}
	}
}

}
}
