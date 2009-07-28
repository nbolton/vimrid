/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 22-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef GLUTKEYBOARDINFO_H_
#define GLUTKEYBOARDINFO_H_

#define V_GLUT_SPECIAL_KEY_MAX 0x006C // Last special GLUT key.
#define V_GLUT_CHAR_KEY_MAX 127 // Last ASCII char number.

namespace vimrid
{
namespace glut
{

class GlutKeyboardInfo
{
public:
	GlutKeyboardInfo();
	virtual ~GlutKeyboardInfo();
	bool SpecialPressed[V_GLUT_SPECIAL_KEY_MAX];
	bool CharPressed[V_GLUT_CHAR_KEY_MAX];
	int ModifierKeys;
};

}
}

#endif // GLUTKEYBOARDINFO_H_
