#pragma once

#include "common.h"

namespace gl::groups {

// BufferUsageARB
enum class buffer_usage_arb : ::libc_types::u32 {
#ifdef GL_DYNAMIC_COPY
    dynamic_copy = GL_DYNAMIC_COPY,
#endif
#ifdef GL_DYNAMIC_DRAW
    dynamic_draw = GL_DYNAMIC_DRAW,
#endif
#ifdef GL_DYNAMIC_READ
    dynamic_read = GL_DYNAMIC_READ,
#endif
#ifdef GL_STATIC_COPY
    static_copy = GL_STATIC_COPY,
#endif
#ifdef GL_STATIC_DRAW
    static_draw = GL_STATIC_DRAW,
#endif
#ifdef GL_STATIC_READ
    static_read = GL_STATIC_READ,
#endif
#ifdef GL_STREAM_COPY
    stream_copy = GL_STREAM_COPY,
#endif
#ifdef GL_STREAM_DRAW
    stream_draw = GL_STREAM_DRAW,
#endif
#ifdef GL_STREAM_READ
    stream_read = GL_STREAM_READ,
#endif
}; // enum class buffer_usage_arb

} // namespace gl::groups
