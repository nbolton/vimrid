/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 01-May-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "Slider.h"

using namespace std;

namespace vimrid
{
namespace ui
{

Slider::Slider(const string &text)
:
	mText(text)
{
}

Slider::~Slider()
{
}

void Slider::Update()
{
}

void Slider::Render()
{
	TranslateToPosition();
}

}
}
