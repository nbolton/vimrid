/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 07-Apr-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#include "AsyncDicomResult.h"

using namespace std;

namespace vimrid
{
namespace dicom
{

AsyncDicomResult::AsyncDicomResult() :
	mSuccess(false)
{
}

AsyncDicomResult::~AsyncDicomResult()
{
	for (VUInt32 i = 0; i < mImages.size(); i++)
	{
		delete mImages[i];
	}
}

void AsyncDicomResult::SetImages(std::vector<DicomImage*> images)
{
	mSuccess = true;
	mImages = images;
}

void AsyncDicomResult::SetError(const std::string &error)
{
	mSuccess = false;
	mError = error;
}

VBool AsyncDicomResult::IsSuccessful()
{
	return mSuccess;
}

vector<DicomImage*> AsyncDicomResult::GetImages()
{
	if (!mSuccess)
	{
		THROW_VIMRID_EX("Cannot get images because an error occurred.");
	}

	return mImages;
}

const string &AsyncDicomResult::GetError()
{
	return mError;
}

}
}
