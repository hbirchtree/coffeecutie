#pragma once

#include "common.h"

namespace gl::group {

// ClearBufferMask
enum class clear_buffer_mask : u32
{
#ifdef GL_ACCUM_BUFFER_BIT
    accum_buffer_bit = GL_ACCUM_BUFFER_BIT,
#endif
#ifdef GL_COLOR_BUFFER_BIT
    color_buffer_bit = GL_COLOR_BUFFER_BIT,
#endif
#ifdef GL_COVERAGE_BUFFER_BIT_NV
    coverage_buffer_bit_nv = GL_COVERAGE_BUFFER_BIT_NV,
#endif
#ifdef GL_DEPTH_BUFFER_BIT
    depth_buffer_bit = GL_DEPTH_BUFFER_BIT,
#endif
#ifdef GL_STENCIL_BUFFER_BIT
    stencil_buffer_bit = GL_STENCIL_BUFFER_BIT,
#endif
}; // enum class clear_buffer_mask
C_FLAGS(clear_buffer_mask, ::libc_types::u32);

} // namespace gl::group
