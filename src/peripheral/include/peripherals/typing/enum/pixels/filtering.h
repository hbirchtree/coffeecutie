#pragma once

#include <peripherals/enum/helpers.h>
#include <peripherals/libc/types.h>

#undef None

namespace typing {

enum class Filtering : libc_types::u8
{
    None,
    Nearest,
    Linear,
};
}
