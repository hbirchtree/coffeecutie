#pragma once

#include "common.h"

namespace gl::group {

// BufferUsageARB
enum class buffer_usage_arb : u32
{
    dynamic_copy = 0x88EA, // GL_DYNAMIC_COPY
    dynamic_draw = 0x88E8, // GL_DYNAMIC_DRAW
    dynamic_read = 0x88E9, // GL_DYNAMIC_READ
    static_copy  = 0x88E6, // GL_STATIC_COPY
    static_draw  = 0x88E4, // GL_STATIC_DRAW
    static_read  = 0x88E5, // GL_STATIC_READ
    stream_copy  = 0x88E2, // GL_STREAM_COPY
    stream_draw  = 0x88E0, // GL_STREAM_DRAW
    stream_read  = 0x88E1, // GL_STREAM_READ
}; // enum class buffer_usage_arb

} // namespace gl::group
