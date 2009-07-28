#ifndef DICOMIMAGE_H_
#define DICOMIMAGE_H_

#include <string>
#include "../types/types.h"
#include "../imaging/VImage.h"
#include "../imaging/ImageMatrix.h"

#define V_BYTE_LENGTH 8
#define V_SHORT_LENGTH 16

namespace vimrid
{
namespace dicom
{

class DicomImage : public vimrid::imaging::VImage
{
public:
	DicomImage(
		VUInt32 rows,
		VUInt32 columns,
		VUInt32 channels,
		VByte bitLength,
		VInt32 *pixels);
	DicomImage(const DicomImage &other);
	DicomImage(std::string serialised);
	virtual ~DicomImage();
	std::string GetSerialised();
	VByte GetBitLength();
	//virtual VInt32 *CopyPixelsToInt32();
	virtual VInt32 *CopyPixelsToInt32() const;
	vimrid::imaging::ImageMatrix *ToImageMatrix();
	void UpdateFromImageMatrix(vimrid::imaging::ImageMatrix matrix);
	//virtual VInt32 &GetPixelAt(VUInt32 row, VUInt32 column, VUInt32 channel);
	virtual VInt32 &Get(VUInt32 row, VUInt32 column, VUInt32 channel) const;
	void ChangeBitLength(VByte bitLength);
	static std::string EndFile;
	static std::string EndStream;
	static std::string Error;

private:
	VByte mBitLength;
	VInt32 *mPixelData;
	std::string getValueFromSerial(std::string header, std::string name);
	VUInt32 stringToInt(std::string s);
	VInt32 *deserialisePixels(std::string pixelDataString);
	std::string getSerialisedPixels();
	void allocatePixelData();
};

}
}

#endif // DICOMIMAGE_H_
