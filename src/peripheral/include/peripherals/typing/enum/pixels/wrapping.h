#pragma once

#include <peripherals/enum/helpers.h>
#include <peripherals/libc/types.h>

namespace typing {
enum class WrapPolicy : libc_types::u8
{
    Repeat,
    MirrorRepeat,
    Clamp,
    MirrorClamp,
};
}
