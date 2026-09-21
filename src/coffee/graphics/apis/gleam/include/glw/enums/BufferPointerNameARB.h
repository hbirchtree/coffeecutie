#pragma once

#include "common.h"

namespace gl::group {

// BufferPointerNameARB
enum class buffer_pointer_name_arb : u32
{
    buffer_map_pointer     = 0x88BD, // GL_BUFFER_MAP_POINTER
    buffer_map_pointer_arb = 0x88BD, // GL_BUFFER_MAP_POINTER_ARB
}; // enum class buffer_pointer_name_arb

} // namespace gl::group
