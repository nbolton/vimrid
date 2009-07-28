/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 01-May-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef SLIDER_H_
#define SLIDER_H_

#include "Control.h"
#include <string>

namespace vimrid
{
namespace ui
{

class Slider : public Control
{
public:
	Slider(const std::string &text);
	virtual ~Slider();
	virtual void Update();
	virtual void Render();
private:
	std::string mText;
};

}
}

#endif // SLIDER_H_
