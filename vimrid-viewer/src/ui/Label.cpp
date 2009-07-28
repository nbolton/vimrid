/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 01-May-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "Label.h"
#include <GL/glut.h>

using namespace std;

namespace vimrid
{
namespace ui
{

Label::Label(const string &text)
:
	TextControl(text)
{
}

Label::~Label()
{
}

void Label::Update()
{
	this->TextControl::Update();
}

void Label::Render()
{
	TranslateToPosition();
	this->TextControl::Render();
}

}
}
