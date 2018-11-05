#pragma once

#include <peripherals/enum/helpers.h>
#include <peripherals/libc/types.h>

namespace semantic {

enum class TypeEnum : u8
{
    UByte,
    Byte,
    UShort,
    Short,
    UInt,
    Int,
    ULL,
    LL,
    Scalar,
    BigScalar,
};
}
