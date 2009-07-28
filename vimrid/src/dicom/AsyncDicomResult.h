/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 07-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef ASYNCDICOMRESULT_H_
#define ASYNCDICOMRESULT_H_

#include <string>
#include "../types/types.h"
#include "DicomImage.h"
#include "../VimridException.h"
#include <vector>

namespace vimrid
{
namespace dicom
{

class AsyncDicomResult
{
public:
	AsyncDicomResult();
	virtual ~AsyncDicomResult();
	void SetImages(std::vector<DicomImage*> images);
	void SetError(const std::string &error);
	VBool IsSuccessful();
	std::vector<DicomImage*> GetImages();
	const std::string &GetError();
private:
	VBool mSuccess;
	std::vector<DicomImage*> mImages;
	std::string mError;
};

}
}

#endif // ASYNCDICOMRESULT_H_
