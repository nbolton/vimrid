/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 20-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef SPRITE_H_
#define SPRITE_H_

#include <vimrid.h>
#include "../VimridUtility.h"

namespace vimrid
{
namespace ui
{
class Control;
}
namespace sprites
{

class Sprite
{
public:
	Sprite();
	Sprite(const VUInt32 &renderIndex);
	virtual ~Sprite();
	const VPoint3f &GetPosition() const { return mPosition; }
	void SetPosition(const VPoint3f &position) { mPosition = position; }
	void SetPosition(const VPoint2f &position) { mPosition = position; }
	const VSizeF &GetSize() const { return mSize; }
	void SetSize(const VSizeF &size) { mSize = size; }
	const VUInt32 &GetRenderIndex() const { return mRenderIndex; }
	void SetRenderIndex(const VUInt32 &renderIndex) { mRenderIndex = renderIndex; }
	void Show();
	void Hide();
	VBool IsVisible();
	const VimridUtility &GetUtility() const;
	VBool CollidesWith(const VPoint3f &position) const;
	VBool CollidesWith(const VPoint2f &position) const;
	VBool CollidesWith(const Sprite &sprite) const;
	void SetParent(Sprite &parent) { mParent = &parent; }
	Sprite &GetParent() { return *mParent; }
	VBool HasParent() { return mParent != NULL; }
	Sprite *GetParentPtr() { return mParent; }
	void TriggerUpdate();
	void TriggerRender();
protected:
	void DrawVertex(VPoint3f point) const;
	void TranslateToPosition() const { TranslateToPosition(mPosition); }
	void TranslateToPosition(const VPoint2f &position) const;
	void TranslateToPosition(const VPoint3f &position) const;
	void ChangeColour(const VColour4F &colour) const;
	virtual void OnChildHidden(Sprite &child) { }
	virtual void OnChildShown(Sprite &child) { }
	virtual void OnShown();
	virtual void OnHidden();
	virtual void Update() = 0;
	virtual void Render() = 0;
private:
	Sprite *mParent;
	VPoint3f mPosition;
	VSizeF mSize;
	VUInt32 mRenderIndex;
	VBool mVisible;
	VBool mShowPending;
	VBool mHidePending;
};

}
}

#endif // SPRITE_H_
