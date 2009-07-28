#include <iostream>
#include <vimrid.h>
#include "VimridLauncher.h"

using namespace std;
using namespace vimrid;

int main(int argc, char* argv[])
{
	try
	{
		VimridLauncher launcher;
		return launcher.Launch(argc, argv);
	}
	catch (VimridException ex)
	{
		string message = ex.Message;
		if (message == "")
		{
			message = "Exception has no message.";
		}
		cerr << "Error: " << message << endl;
		return ERROR_EXCEPTION;
	}
}
