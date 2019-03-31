#pragma once

#include <peripherals/enum/helpers.h>
#include <peripherals/libc/types.h>

namespace typing {
namespace graphics {

enum class framebuffer_type
{
    None,
    Draw = 0x1,
    Read = 0x2,
    All  = 0x3,
};

C_FLAGS(framebuffer_type, libc_types::u8);

} // namespace graphics
} // namespace typing
