/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 05-May-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef CONTAINER_H_
#define CONTAINER_H_

#include "Control.h"

namespace vimrid
{
namespace ui
{

class Container: public vimrid::ui::Control
{
public:
	Container(VInt32 renderIndex);
	virtual ~Container();
	virtual void Update();
	virtual void Render();
	VBool IsMenuToggleSuspended();
	VPoint3f GetCursorPosition() const;
	void ResetCursor();
	void CentreCursor(Sprite &sprite);
protected:
	virtual void OnChildHidden(Sprite &child);
	virtual void OnChildShown(Sprite &child);
private:
	VPoint3f mCursorOffset;
	VBool mCursorCentred;
};

}
}

#endif // CONTAINER_H_
