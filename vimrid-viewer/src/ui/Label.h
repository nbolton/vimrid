/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 01-May-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef LABEL_H_
#define LABEL_H_

#include "TextControl.h"
#include <string>

namespace vimrid
{
namespace ui
{

class Label : public TextControl
{
public:
	Label(const std::string &text);
	virtual ~Label();
	virtual void Update();
	virtual void Render();
};

}
}

#endif // LABEL_H_
