/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 02-May-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef TEXTCONTROL_H_
#define TEXTCONTROL_H_

#include "Control.h"
#include <string>

namespace vimrid
{
namespace ui
{

class TextControl: public Control
{
public:
	TextControl(const std::string &text);
	virtual ~TextControl();
	virtual void Update();
	virtual void Render();
    VColour4F GetTextColour() const;
    void SetTextColour(const VColour4F &textColour) { mTextColour = textColour; }
    virtual void OnSelectPress();
    virtual void OnSelectRelease();
    virtual void OnSelectReset();
    const std::string &GetText() const { return mText; }
    void SetText(const std::string &text) { mText = text; }
private:
	std::string mText;
	void* mFont;
	VPoint2f mTextOffset;
	VColour4F mTextColour;
};

}
}

#endif // TEXTCONTROL_H_
