#pragma once

#include <peripherals/libc/types.h>

namespace typing {
enum class PixCmp : libc_types::u8
{
    None,

    R,
    G,
    B,
    A,
    RG,
    RGB,
    RGBA,
    BGR,
    BGRA,

    Stencil,
    Depth,
    DepthStencil,
};
}
