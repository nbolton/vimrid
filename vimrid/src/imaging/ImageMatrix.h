#ifndef IMAGEMATRIX_H_
#define IMAGEMATRIX_H_

#include <map>
#include "../types/types.h"
#include "ImageMatrix.h"
#include "ImageMatrixRow.h"
#include "ImageMatrixColumn.h"
#include "VImage.h"

namespace vimrid
{
namespace imaging
{

class ImageMatrix : public VImage
{
public:
	ImageMatrix();
	ImageMatrix(const ImageMatrix &other);
	ImageMatrix(
		VUInt32 rowCount,
		VUInt32 columnCount,
		VUInt32 channelCount);
	virtual ~ImageMatrix();
	//ImageMatrixRow &operator[](VUInt32 rowIndex);
	VInt32 GetPixelMin() const;
	VInt32 GetPixelMax() const;
	ImageMatrix *CopyEmpty() const;
	virtual VInt32 *CopyPixelsToInt32() const;
	//virtual VInt32 &GetPixelAt(VUInt32 row, VUInt32 column, VUInt32 channel);
	virtual VInt32 &Get(VUInt32 row, VUInt32 column, VUInt32 channel) const;
	ImageMatrix *GetCombinedCopy(ImageMatrix &otherImage) const;
	void CombineWith(ImageMatrix &otherImage);
	ImageMatrix *GetNormalisedCopy(VInt32 targetMin, VInt32 targetMax) const;
	void Normalise(VInt32 targetMin, VInt32 targetMax);
	void Scope(VFloat scopeMin, VFloat scopeMax);
	template<class T> T *GetRotatedCopy(VDouble degrees) const;
	void PrintDebugView() const;
	void PrintDebugFile(const std::string &fileName) const;
	void LoadPixels(const VInt32 *source);
	const VInt32 *GetRawPixelData() const { return mPixelData; }
	void ZeroPixels();
private:
	//ImageMatrixRowMap rows;
	VInt32 *mPixelData;
	void _copyRotated(VDouble angle, ImageMatrix &target) const;
	void assertImageDimensions(const ImageMatrix &otherImage) const;
	void allocatePixelData();
};

template<class T>
T *ImageMatrix::GetRotatedCopy(VDouble degrees) const
{
	ImageMatrix *copy = CopyEmpty();
	_copyRotated(degrees, *copy);
	return (T*)copy;
}

}
}

#endif // IMAGEMATRIX_H_
