#pragma once

#include "common.h"

namespace gl::group {

// BufferPointerNameARB
enum class buffer_pointer_name_arb : ::libc_types::u32
{
#ifdef GL_BUFFER_MAP_POINTER
    buffer_map_pointer = GL_BUFFER_MAP_POINTER,
#endif
#ifdef GL_BUFFER_MAP_POINTER_ARB
    buffer_map_pointer_arb = GL_BUFFER_MAP_POINTER_ARB,
#endif
}; // enum class buffer_pointer_name_arb

} // namespace gl::group
