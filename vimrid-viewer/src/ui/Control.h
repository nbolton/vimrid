/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 01-May-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef CONTROL_H_
#define CONTROL_H_

#include "../sprites/Sprite.h"
#include <map>
#include <string>
#include <vector>

namespace vimrid
{

class VimridUtility;

namespace ui
{

class Control : public vimrid::sprites::Sprite
{
public:
	Control();
	Control(VUInt32 renderIndex);
	virtual ~Control();
	virtual void Update() = 0;
	virtual void Render() = 0;
	void AddChild(Control &child);
	void RenderChildren();
	void UpdateChildren();
    VColour4F GetBackground() const;
    void SetBackground(const VColour4F &value) { mBackground = value; }
    VBool IsHighlighted() const { return mHighlighted; }
    void Highlight() { mHighlighted = true; }
    void Unhighlight() { mHighlighted = false; }
    VBool IsSelected() const { return mSelected; }
    virtual void OnSelectPress();
    virtual void OnSelectRelease();
    virtual void OnSelectReset();
	void SetName(const std::string &name) { mName = name; }
	const std::string &GetName() const { return mName; }
	virtual void OnChildSelectPress(Control &child);
	virtual void OnChildSelectRelease(Control &child);
	VBool HasParentControl();
	Control &GetParentControl();
	Control *getParentControlPtr();
	VBool HasVisibleChild();
	void Enable() { mEnabled = true; }
	void Disable() { mEnabled = false; }
	VBool IsEnabled() const { return mEnabled; }
protected:
	const std::vector<Control*> &GetChildren() const { return mChildren; }
	void DrawRectangle(const VPoint2f &position, const VSizeF &size) const;
	void DrawRectangleOutline(
		const VPoint2f &position,
		const VSizeF &size,
		VBool bevelColour,
		VBool invertBevel) const;
private:
	std::vector<Control*> mChildren;
	VColour4F mBackground;
	VColour4F mHighlightColour;
	std::string mName;
	VBool mSelected;
	VBool mEnabled;
	VBool mHighlighted;
};

}
}

#endif // CONTROL_H_
