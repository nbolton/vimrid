#ifndef DICOMTCPSERVER_H_
#define DICOMTCPSERVER_H_

#include <string>

namespace vimrid
{
namespace dicom
{
namespace server
{

class DicomTcpServer
{
public:
	DicomTcpServer(const std::string &basePath);
	virtual ~DicomTcpServer();
	void Run();
private:
	std::string mBasePath;
    void writeSocketError(int & clientSocketFD);
};

}
}
}

#endif // DICOMTCPSERVER_H_
