/*
 * InterfaceDemo1.h
 *
 *  Created on: 02-Apr-2009
 *      Author: nick
 */

#ifndef INTERFACEDEMO1_H_
#define INTERFACEDEMO1_H_

#include "../../glut/GlutApplication.h"
#include "../cubedemos/TestCube.h"

namespace vimrid
{
namespace demos
{
namespace interface
{

class InterfaceDemo1: public vimrid::glut::GlutApplication
{
public:
	InterfaceDemo1(VimridSettings &settings);
	virtual ~InterfaceDemo1();
protected:
	void Init();
	void Update();
	void Render();
	void Reshape();
private:
	void createCube();
	void drawCube();
	vimrid::demos::cubedemos::TestCube cube;
};

}
}
}

#endif // INTERFACEDEMO1_H_
