/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 07-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "DicomTcpClient.h"
#include "AsyncDicomTask.h"
#include "../Debug.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <vector>

#define BUFFER_SIZE 256

using namespace std;

namespace vimrid
{
namespace dicom
{

VUInt32 DicomTcpClient::mDownloadTaskCounter = 0;
vector<AsyncDicomTask*> DicomTcpClient::mDownloadTasks;

DicomTcpClient::DicomTcpClient(const string &hostName, int port)
:
	mHostName(hostName),
	mPort(port),
	mDownloadCallback(NULL)
{
}

DicomTcpClient::~DicomTcpClient()
{
}

string DicomTcpClient::GetHostName()
{
	return mHostName;
}

int DicomTcpClient::GetPort()
{
	return mPort;
}

void DicomTcpClient::DownloadAsync(const string &path)
{
	if (path == "")
	{
		THROW_VIMRID_EX("Cannot download file or directory with no path specified.");
	}

	if (mDownloadCallback == NULL)
	{
		THROW_VIMRID_EX("DICOM TCP client download callback was not set.");
	}

	// Each task can be accessed by an ID.
	VUInt32 taskId = mDownloadTaskCounter++;

	// Create a new task so we can add this so the active tasks.
	AsyncDicomTask *task = new AsyncDicomTask(taskId, path, *this);

	// Store the task in a static place so we can get file name when in thread.
	mDownloadTasks.push_back(task);

	// Invoke a new thread which calls a static function outside this class.
	pthread_t *thread = new pthread_t();
	void *taskIdPtr = &const_cast<VUInt32&>(task->GetTaskId());
	int result = pthread_create(thread, NULL, download, taskIdPtr);
	if (result < 0)
	{
		THROW_VIMRID_EX("Unable to create thread.");
	}

	// Remember the thread so we can deallocate the memory later.
	task->SetThread(thread);
}

AsyncDicomTask &DicomTcpClient::getTask(VUInt32 taskId)
{
	DEBUG("Getting task (ID: " << taskId << ") in vector with "
		<< mDownloadTasks.size() << " elements.");

	AsyncDicomTask *result = NULL;
	for (size_t i = 0; i < mDownloadTasks.size(); i++)
	{
		AsyncDicomTask &task = *mDownloadTasks[i];
		DEBUG("Checking task ID: " << task.GetTaskId());
		if (task.GetTaskId() == taskId)
		{
			result = &task;
			DEBUG("Task with ID " << task.GetTaskId() << " will be used.");
		}
	}
	if (result == NULL)
	{
		THROW_VIMRID_EX("Unable to get async DICOM task with ID: " << taskId << ".");
	}
	return *result;
}

AsyncDicomTask &DicomTcpClient::getTask(void *taskIdPtr)
{
	return getTask(getTaskId(taskIdPtr));
}

void DicomTcpClient::SetDownloadCallback(void *(*callback)(void*))
{
	mDownloadCallback = callback;
}

void DicomTcpClient::RunDownloadCallback(void *taskIdPtr)
{
	mDownloadCallback(taskIdPtr);
}

VUInt32 DicomTcpClient::getTaskId(void *taskIdPtr)
{
	// Safely convert memory reference to value.
	return *(VUInt32*)taskIdPtr;
}

void *DicomTcpClient::download(void *taskIdPtr)
{
	// Instead of working in the static function, pass request to client object.
	getTask(taskIdPtr).GetClient().HandleAsyncTask(getTaskId(taskIdPtr));

	// Not sure why we need to return something.
	return NULL;
}

void DicomTcpClient::HandleAsyncTask(VUInt32 taskId)
{
	/* It's important here to use & before the variable name
	 * which makes it a reference; this is still using stack
	 * memory, but without the & it would be using stack memory
	 * local only to this function (which would get cleaned up
	 * once it's left).
	 */
	AsyncDicomTask &task = getTask(taskId);
	AsyncDicomResult &result = *new AsyncDicomResult();

	try
	{
		// Call the non-async download function as normal.
		result.SetImages(Download(task.GetPath()));
	}
	catch (VimridException ex)
	{
		result.SetError(ex.Message);
	}

	task.SetResult(&result);
}

AsyncDicomResult *DicomTcpClient::GetResult(void *taskIdPtr)
{
	// Get a pointer to the task, and return it's result.
	return getTask(taskIdPtr).GetResult();
}

vector<DicomImage*> DicomTcpClient::Download(const string &path)
{
	// Buffer for read and write.
	char tempBuffer[BUFFER_SIZE];

	WRITE_DEBUG_TEXT("Connecting to " << mHostName << ":" << mPort << "...");

	int socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFD < 0)
	{
		THROW_VIMRID_EX("Could not open socket.");
	}

	struct hostent *server;
	server = gethostbyname(mHostName.c_str());
	if (server == NULL)
	{
		THROW_VIMRID_EX("Host '" << mHostName << "' does not exist.");
	}

	struct sockaddr_in serverAddress;
	bzero((char*)&serverAddress, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	bcopy((char*)server->h_addr, (char*)&serverAddress.sin_addr.s_addr, server->h_length);

	serverAddress.sin_port = htons(mPort);
	int connectResult = connect(
		socketFD,
		(const sockaddr*)&serverAddress,
		sizeof(serverAddress));

	if (connectResult < 0)
	{
		THROW_VIMRID_EX("Could not connect to host ("
			<< mHostName << ":" << mPort << ").");
	}

	WRITE_DEBUG_TEXT("Requesting file name " << path << "...");

	int writeResult = write(socketFD, path.c_str(), path.length());
	if (writeResult < 0)
	{
		THROW_VIMRID_EX("Could not write to socket for DICOM file name.");
	}

	WRITE_DEBUG_TEXT("Downloading DICOM data from server...");

	VBool readTcpStream = true;
	stringstream serialStream;

	while (readTcpStream)
	{
		// Read a bit at a time, eventually "end" string will be received.
		bzero(tempBuffer, BUFFER_SIZE);
		int readResult = read(socketFD, tempBuffer, BUFFER_SIZE - 1);
		if (readResult < 0)
		{
			THROW_VIMRID_EX("Could not read from socket.");
		}

		// Copy char buffer to string for easier usage.
		string tempBufferString = tempBuffer;

		// Append the received data to the existing data.
		serialStream << tempBufferString;

		// Continue reading stream while end is not found.
		readTcpStream = tempBufferString.find(DicomImage::EndStream) == string::npos;

		// If an error occurs, break the loop and quit.
		if (tempBufferString.find(DicomImage::Error) != string::npos)
		{
			THROW_VIMRID_EX("Cannot get DICOM data (server error).");
		}
	}

	/*ofstream file;
	file.open("/home/nick/tmp/dicom-tcp-client.out");
	file << serialStream.str() << endl;
	file.close();*/

	// Copy stream to string to make it easier to separate.
	string serialString = serialStream.str();

	VBool readSerialString = true;
	vector<string> dicomStrings;
	VUInt64 end = 0;
	VUInt64 start = 0;

	// TODO: This is open to being an infinite loop.
	while (readSerialString)
	{
		/* Calculate where to read up to, based on the last index that the
		 * serial string started from. */
		end = serialString.find(DicomImage::EndFile, start);

		// While the end of the string has not been reached, get the next segment.
		if (end != string::npos)
		{
			// Append the terminator so that the next pass does not find the same terminator.
			end += string(DicomImage::EndFile).size();

			// Get the single serialisation string for the current DICOM file.
			string singleSerialString = serialString.substr(start, end - start);

			// Add the string to the vector to be deserialised in the next loop.
			dicomStrings.push_back(singleSerialString);

			// Move the next start position to where this left off.
			start += singleSerialString.length();
		}
		else
		{
			// Otherwise, there must not be anything else to read.
			readSerialString = false;
		}
	}

	int closeResult = close(socketFD);
	if (closeResult < 0)
	{
		THROW_VIMRID_EX("Could not close connection to server.");
	}

	vector<DicomImage*> images;
	for (VUInt32 i = 0; i < dicomStrings.size(); i++)
	{
		/* Create image object which deserialises the pixels, and push
		 * the pointer to the back of the images vector (to be returned). */
		images.push_back(new DicomImage(dicomStrings[i]));
		WRITE_DEBUG_TEXT("DICOM image downloaded successfully.");
	}

	return images;
}

}
}
