#include "DicomTcpServer.h"
#include "DicomReader.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include <vimrid.h>
#include <imebra.h>

#include <sys/stat.h>
#include <vector>
#include <dirent.h>
#include <algorithm>

#define TEMP_PORT 1234
#define BUFFER_SIZE 256

using namespace std;
using namespace puntoexe;
using namespace vimrid::imaging;

namespace vimrid
{
namespace dicom
{
namespace server
{

DicomTcpServer::DicomTcpServer(const string &basePath)
	: mBasePath(basePath)
{
}

DicomTcpServer::~DicomTcpServer()
{
}

void DicomTcpServer::writeSocketError(int & clientSocketFD)
{
	string error = DicomImage::Error;
	write(clientSocketFD, error.c_str(), error.size());
}

void DicomTcpServer::Run()
{
	int port = TEMP_PORT;
	char buffer[BUFFER_SIZE];
	int serverSocketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocketFD < 0)
	{
		THROW_VIMRID_EX("Unable to open socket.");
	}

	struct sockaddr_in serverAddress;
	bzero((char*)(&serverAddress), sizeof (serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(port);
	int bindResult = bind(serverSocketFD, (struct sockaddr*)(&serverAddress),
	sizeof(serverAddress));

	if (bindResult < 0)
	{
		THROW_VIMRID_EX("Could not to bind to address.");
	}

	while(true)
	{
		VBool dropSocket = false;

		WRITE_DEBUG_TEXT("Waiting for client connections on port " << port << "...");

		listen(serverSocketFD, 5);

		struct sockaddr_in clientAddress;
		socklen_t clientLength = (socklen_t)sizeof(clientAddress);
		int clientSocketFD = accept(
			serverSocketFD,
			(struct sockaddr*)&clientAddress,
			&clientLength);

		if (clientSocketFD < 0)
		{
			WRITE_DEBUG_TEXT("Warning: Could not accept connection.");
			continue;
		}

		WRITE_DEBUG_TEXT("Client connection accepted.");

		bzero(buffer, BUFFER_SIZE);
		int readResult = read(clientSocketFD, buffer, BUFFER_SIZE);
		if (readResult < 0)
		{
			WRITE_DEBUG_TEXT("Warning: Could not read from socket.");
			continue;
		}

		// Find out what type of path is requested.
		string path = mBasePath + "/" + buffer;
		struct stat status;
		stat(path.c_str(), &status);

		// Is the path a directory (DIR) or a regular file (REG)?
		VBool isDir = (status.st_mode & S_IFDIR);
		VBool isFile = (status.st_mode & S_IFREG);

		vector<string> paths;
		if (isDir)
		{
			WRITE_DEBUG_TEXT("Reading DICOM directory (" << path << ")... ");

			DIR *dir = opendir(path.c_str());
			if (dir != NULL)
			{
				struct dirent *dirent;
				while ((dirent = readdir(dir)) != NULL)
				{
					string fileName = dirent->d_name;

					/* TODO: This only checks for . and .. but does not ensure that
					 * the file added to the list is actually a DICOM file. */
					if ((fileName.compare(".") != 0) && (fileName.compare("..") != 0))
					{
						/* Record the full path to avoid having complicated combinations
						 * of file paths later on. */
						paths.push_back(path + "/" + fileName);
					}
				}

				// Finally, close the director.
				closedir(dir);

				WRITE_DEBUG_TEXT("Number of files found: " << paths.size());
			}
			else
			{
				WRITE_DEBUG_TEXT("Error: Unable to read from directory.");
				writeSocketError(clientSocketFD);
				dropSocket = true;
			}
		}
		else if (isFile)
		{
			paths.push_back(path);
		}
		else
		{
			WRITE_DEBUG_TEXT("Error: Path (" << path << ") is neither a directory or a file.");
			writeSocketError(clientSocketFD);
			dropSocket = true;
		}

		if (!dropSocket)
		{
			// Sort the files so they appear in order on the client.
			sort(paths.begin(), paths.end());

			DicomReader dicomReader;
			stringstream serialStream;

			for (VUInt32 i = 0; i < paths.size(); i++)
			{
				try
				{
					string fileName = paths[i];

					WRITE_DEBUG_TEXT("Reading DICOM file (" << fileName << ")... ");
					DicomImage *dicomImage = dicomReader.LoadImage(fileName);
					WRITE_DEBUG_TEXT("DICOM file read successful.");

					WRITE_DEBUG_TEXT("Sending serialised data... ");
					serialStream << dicomImage->GetSerialised();
					WRITE_DEBUG_TEXT("Added to serialised stream.");

					// Delete the image as it's no longer required.
					delete dicomImage;
				}
				catch (streamExceptionOpen& ex)
				{
					WRITE_DEBUG_TEXT("Error: Could not open specified DICOM file.");
					writeSocketError(clientSocketFD);
					dropSocket = true;
					break;
				}
			}

			if (!dropSocket)
			{
				// Tell the client when to stop reading.
				serialStream << DicomImage::EndStream;

				/*ofstream file;
				file.open("/home/nick/tmp/dicom-tcp-server.out");
				file << serialStream.str() << endl;
				file.close();*/

				// Copy to string so it's easier to measure length and convert to char array.
				string serialString = serialStream.str();

				int headerResult = write(
					clientSocketFD,
					serialString.c_str(),
					serialString.length());

				if (headerResult < 0)
				{
					WRITE_DEBUG_TEXT(
						"Warning: Could not write serialised " <<
						"data to client socket.");
				}
			}
		}

		int closeResult = close(clientSocketFD);
		if (closeResult < 0)
		{
			THROW_VIMRID_EX("Could not close connection to client.");
		}
	}
}

}
}
}
