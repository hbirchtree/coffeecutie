#pragma once

#include <coffee/core/libc_types.h>

namespace Coffee {
namespace Display {
struct ColorSpace
{
    ColorSpace() : red(0), green(0), blue(0)
    {
    }

    u8 red   = 0;
    u8 green = 0;
    u8 blue  = 0;
    u8 extra = 0;
};
} // namespace Display
} // namespace Coffee
