#pragma once

#include <peripherals/libc/types.h>

namespace typing {
enum class pix_components : libc_types::u8
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
