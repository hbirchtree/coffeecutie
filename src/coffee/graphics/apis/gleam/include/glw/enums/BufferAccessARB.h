#pragma once

#include "common.h"

namespace gl::group {

// BufferAccessARB
enum class buffer_access_arb : u32
{
    read_only      = 0x88B8, // GL_READ_ONLY
    read_write     = 0x88BA, // GL_READ_WRITE
    write_only     = 0x88B9, // GL_WRITE_ONLY
    write_only_oes = 0x88B9, // GL_WRITE_ONLY_OES
}; // enum class buffer_access_arb

} // namespace gl::group
