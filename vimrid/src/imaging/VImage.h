#ifndef VIMAGE_H_
#define VIMAGE_H_

#include "../types/types.h"
#include "../VimridException.h"

namespace vimrid
{
namespace imaging
{

/* This is the root of all images in the ViMRID system, both ImageMatrix
 * and DicomImage inherit from this class. It provides common functions
 * for getting information for dimensions and virtual methods for getting
 * pixels at specific locations.
 */
class VImage
{
public:
	VImage();
	VImage(const VImage &other);
	VImage(
		VUInt32 rowCount,
		VUInt32 columnCount,
		VUInt32 channelCount);
	virtual ~VImage();
	virtual VInt32 *CopyPixelsToInt32() const = 0;
	VUByte *CopyPixelsToUByte() const;
	VUInt32 GetColumnCount() const;
	VUInt32 GetRowCount() const;
	VUInt32 GetChannelCount() const;
	VUInt32 GetPixelCount() const;
	VUInt32 GetWidth() const;
	VUInt32 GetHeight() const;
	virtual VInt32 &Get(VUInt32 row, VUInt32 column, VUInt32 channel) const = 0;
	void UpdateDimnensions(VUInt32 rowCount, VUInt32 columnCount, VUInt32 channelCount);
protected:
	void SetColumnCount(VUInt32 columnCount);
	void SetRowCount(VUInt32 rowCount);
	void SetChannelCount(VUInt32 channelCount);
private:
	VUInt32 mRowCount;
	VUInt32 mColumnCount;
	VUInt32 mChannelCount;
};

}
}

#endif // VIMAGE_H_
