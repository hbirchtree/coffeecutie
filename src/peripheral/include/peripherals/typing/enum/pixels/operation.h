#pragma once

#include <peripherals/enum/helpers.h>
#include <peripherals/libc/types.h>

namespace typing {
enum class PixelOperation : libc_types::u8
{
    SwapEndiannes,
    Alignment,
};
}
