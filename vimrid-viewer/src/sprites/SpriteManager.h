/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 20-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef SPRITEMANAGER_H_
#define SPRITEMANAGER_H_

#include <vector>
#include "Sprite.h"

namespace vimrid
{
namespace sprites
{

class SpriteManager
{
public:
	SpriteManager();
	virtual ~SpriteManager();
	void Update();
	void Render();
	void Add(Sprite &sprite);
	void Remove(Sprite *sprite);
	VUInt32 GetSpriteCount() const { return mSprites.size(); }
private:
	std::vector<Sprite*> mSprites;
	static bool sortSpriteByRenderIndex(Sprite *a, Sprite *b);
};

}
}

#endif // SPRITEMANAGER_H_
