#pragma once

#include "common.h"

namespace gl::group {

// ClearBufferMask
enum class clear_buffer_mask : u32
{
    accum_buffer_bit       = 0x00000200, // GL_ACCUM_BUFFER_BIT
    color_buffer_bit       = 0x00004000, // GL_COLOR_BUFFER_BIT
    depth_buffer_bit       = 0x00000100, // GL_DEPTH_BUFFER_BIT
    stencil_buffer_bit     = 0x00000400, // GL_STENCIL_BUFFER_BIT
    coverage_buffer_bit_nv = 0x00008000, // GL_COVERAGE_BUFFER_BIT_NV
}; // enum class clear_buffer_mask
C_FLAGS(clear_buffer_mask, u32);

} // namespace gl::group
