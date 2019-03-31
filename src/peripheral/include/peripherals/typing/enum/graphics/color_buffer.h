#pragma once

#include <peripherals/enum/helpers.h>
#include <peripherals/identify/compiler/function_inlining.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/type_safety.h>

namespace typing {
namespace graphics {

enum class color_buffer
{
    None    = 0x0,
    Color   = 0x1,
    Depth   = 0x2,
    Stencil = 0x4,

    DepthStencil = 0x6,
};

C_FLAGS(color_buffer, libc_types::u32);
} // namespace graphics
} // namespace typing
