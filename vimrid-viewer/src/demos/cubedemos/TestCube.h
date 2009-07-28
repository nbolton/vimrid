/* Uses test cube from http://www.gamedev.net/reference/articles/article1681.asp
 */

#ifndef TESTCUBE_H_
#define TESTCUBE_H_

namespace vimrid
{
namespace demos
{
namespace cubedemos
{

typedef struct
{
  struct
  {
	float pos[3];
	float col[3];
  } ver[8];

  struct
  {
	unsigned int ver[4];
  } quad[6];
} TestCube;

}
}
}

#endif // TESTCUBE_H_
