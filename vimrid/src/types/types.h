#ifndef VTYPES_H_
#define VTYPES_H_

#include <stdint.h>

namespace vimrid
{

typedef float VFloat;
typedef double VDouble;
typedef bool VBool;

typedef signed char VByte;
typedef unsigned char VUByte;

typedef signed char VChar;
typedef unsigned char VUChar;

typedef int16_t VInt16;
typedef int32_t VInt32;
typedef int64_t VInt64;

typedef uint16_t VUInt16;
typedef uint32_t VUInt32;
typedef uint64_t VUInt64;

}

#include "VPoint2f.h"
#include "VPoint3f.h"
#include "VSize3F.h"
#include "VColour4F.h"

// Legacy support for point types.
#define Point2f vimrid::VPoint2f
#define Point3f vimrid::VPoint3f
#define VSizeF vimrid::VSize3F

#endif // VTYPES_H_
