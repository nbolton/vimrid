/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 07-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef ASYNCDICOMTASK_H_
#define ASYNCDICOMTASK_H_

#include "DicomImage.h"
#include "AsyncDicomResult.h"
#include <string>

namespace vimrid
{
namespace dicom
{

class DicomTcpClient;

class AsyncDicomTask
{
public:
	AsyncDicomTask(int taskId, const std::string &path, DicomTcpClient &client);
	virtual ~AsyncDicomTask();
	const VUInt32 &GetTaskId() const;
	const std::string &GetPath();
	DicomTcpClient &GetClient();
	AsyncDicomResult *GetResult();
	void SetResult(AsyncDicomResult *result);
	void SetThread(pthread_t *thread) { mThread = thread; }
private:
	VUInt32 mTaskId;
	std::string mPath;
	DicomTcpClient &mClient;
	AsyncDicomResult *mResult;
	pthread_t *mThread;
};


}
}

#endif // ASYNCDICOMTASK_H_
