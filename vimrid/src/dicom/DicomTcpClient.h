/* Allows applications to access a ViMRID DICOM TCP server.
 *
 * TODO: Implement caching on local file system, as the
 * DICOM files can be quite heavy to download (as they
 * are uncompressed).
 *
 * TODO: Implement compression for downloads.
 *
 * Created: 07-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef DICOMTCPCLIENT_H_
#define DICOMTCPCLIENT_H_

#include "DicomImage.h"
#include "AsyncDicomResult.h"
#include <string>
#include <pthread.h>
#include <vector>

namespace vimrid
{
namespace dicom
{

class AsyncDicomTask;

class DicomTcpClient
{
public:
	DicomTcpClient(const std::string &hostName, int port);
	virtual ~DicomTcpClient();
	void DownloadAsync(const std::string &path);
	std::string GetHostName();
	int GetPort();
	void SetDownloadCallback(void *(*callback)(void*));
	void RunDownloadCallback(void *taskIdPtr);
	AsyncDicomResult *GetResult(void *taskIdPtr);
	void HandleAsyncTask(VUInt32 taskId);
	std::vector<DicomImage*> Download(const std::string &path);
private:
	std::string mHostName;
	int mPort;
	void *(*mDownloadCallback)(void *);
	static VUInt32 mDownloadTaskCounter;
	static std::vector<AsyncDicomTask*> mDownloadTasks;
	static void *download(void *argPtr);
	static VUInt32 getTaskId(void *taskIdPtr);
	static AsyncDicomTask &getTask(VUInt32 taskId);
	static AsyncDicomTask &getTask(void *taskIdPtr);
};

}
}

#endif // DICOMTCPCLIENT_H_
