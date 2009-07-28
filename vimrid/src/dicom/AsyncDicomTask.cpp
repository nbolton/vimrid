/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 07-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "AsyncDicomTask.h"
#include "DicomTcpClient.h"
#include <iostream>

using namespace std;

namespace vimrid
{
namespace dicom
{

AsyncDicomTask::AsyncDicomTask(
	int taskId,
	const string &path,
	DicomTcpClient &client)
	:
	mTaskId(taskId),
	mPath(path),
	mClient(client)
{
}

AsyncDicomTask::~AsyncDicomTask()
{
	delete mThread;
}

const VUInt32 &AsyncDicomTask::GetTaskId() const
{
	return mTaskId;
}

const string &AsyncDicomTask::GetPath()
{
	return mPath;
}

DicomTcpClient &AsyncDicomTask::GetClient()
{
	/*
 ( ( A* ) 0 )->Blah( );
 int mClient;
 &( ( A* ) 0 )->mClient*/
	return mClient;
}

AsyncDicomResult *AsyncDicomTask::GetResult()
{
	return mResult;
}

void AsyncDicomTask::SetResult(AsyncDicomResult *result)
{
	mResult = result;
	VUInt32 taskId = GetTaskId();
	mClient.RunDownloadCallback(&taskId);
}

}
}
