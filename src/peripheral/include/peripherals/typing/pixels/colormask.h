#pragma once

#include <peripherals/libc/types.h>

namespace typing {
namespace pixels {

struct colormask
{
    bool           r : 1;
    bool           g : 1;
    bool           b : 1;
    bool           a : 1;
    libc_types::u8 padding : 4;
};

} // namespace pixels
} // namespace typing
