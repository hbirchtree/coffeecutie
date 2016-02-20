#pragma once

#include "enumfun.h"
#include "../tdef/integertypes.h"

namespace Coffee{

enum class TypeEnum : uint8
{
    UByte, Byte,
    UShort, Short,
    UInt, Int,
    ULL, LL,
    Scalar, BigScalar,
};

enum class ResourceAccess : uint16
{
    ExclusiveLocking = 0x1,

    ReadOnly = 0x10,
    WriteOnly = 0x20,
    ReadWrite = 0x30,

    Persistent = 0x40,
    Streaming = 0x80,

    Append = 0x100,

    Virtual = 0x200,
};
C_FLAGS(ResourceAccess,uint32);

enum class BitFormat : uint8
{
    Byte, ByteR,
    UByte, UByteR,
    UByte_332, UByte_233R,

    Short, ShortR,
    UShort, UShortR,
    UShort_565, UShort_565R,
    UShort_5551, UShort_1555R,

    Int, IntR,
    UInt, UIntR,
    UInt_5999R,
    UInt_1010102, UInt_2101010R,

    Scalar_16,
    Scalar_32,
    Scalar_64,
    Scalar_11_11_10,

    Scalar_32_Int_24_8,
};

}