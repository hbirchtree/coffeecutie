#pragma once

#include <peripherals/enum/helpers.h>
#include <peripherals/libc/types.h>

namespace semantic {

enum class TypeEnum : libc_types::u8
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

    Packed_UFloat, /* PixFmt::R10FG11FB11F */
};
}
