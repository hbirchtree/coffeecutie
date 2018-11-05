#pragma once

#include <peripherals/enum/helpers.h>
#include <peripherals/libc/types.h>

namespace typing {
enum class WrapPolicy : u8
{
    Repeat,
    MirrorRepeat,
    Clamp,
    MirrorClamp,
};
}
