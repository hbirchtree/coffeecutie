#pragma once

#include "common.h"

namespace gl::group {

// BufferAccessARB
enum class buffer_access_arb : ::libc_types::u32
{
#ifdef GL_READ_ONLY
    read_only = GL_READ_ONLY,
#endif
#ifdef GL_READ_WRITE
    read_write = GL_READ_WRITE,
#endif
#ifdef GL_WRITE_ONLY
    write_only = GL_WRITE_ONLY,
#endif
#ifdef GL_WRITE_ONLY_OES
    write_only_oes = GL_WRITE_ONLY_OES,
#endif
}; // enum class buffer_access_arb

} // namespace gl::group
