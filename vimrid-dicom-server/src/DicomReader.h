#ifndef DICOMREADER_H_
#define DICOMREADER_H_

#include <vimrid.h>

namespace vimrid
{
namespace dicom
{
namespace server
{

class DicomReader
{
public:
	vimrid::dicom::DicomImage *LoadImage(std::string fileName);
};

}
}
}

#endif // DICOMREADER_H_
