#include <iostream>
#include <vimrid.h>
#include "DicomTcpServer.h"

#define ERROR_NONE 0
#define ERROR_EXCEPTION 1

using namespace std;
using namespace vimrid;
using namespace vimrid::dicom::server;

int main(int argc, char *argv[])
{
	try
	{
		string basePath;
		VBool showHelp = false;

		for (int i = 0; i < argc; i++)
		{
			string s = argv[i];
			if (s.compare("--debug") == 0)
			{
				Debug::_enableDebugMode = true;
			}
			else if (s.compare("--base-path") == 0)
			{
				basePath = argv[++i];
			}
			else if (s.compare("--help") == 0)
			{
				showHelp = true;
			}
		}

		if (basePath == "")
		{
			THROW_VIMRID_EX("Base path (--base-path) must be specified with a value.");
		}
		else if(showHelp)
		{
			cout <<
				"--debug              Show debug messages." << endl <<
				"--base-path {value}  Specify the DICOM base path." << endl <<
				endl;
		}

		WRITE_DEBUG_TEXT("Using base path: " << basePath);

		DicomTcpServer server(basePath);
		server.Run();

		return ERROR_NONE;
	}
	catch (VimridException e)
	{
		cerr << "Error: " << e.Message << endl;
		return ERROR_EXCEPTION;
	}
}
