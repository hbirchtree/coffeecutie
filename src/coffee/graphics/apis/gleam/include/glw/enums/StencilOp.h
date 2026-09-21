#pragma once

#include "common.h"

namespace gl::group {

// StencilOp
enum class stencil_op : u32
{
    decr      = 0x1E03, // GL_DECR
    decr_wrap = 0x8508, // GL_DECR_WRAP
    incr      = 0x1E02, // GL_INCR
    incr_wrap = 0x8507, // GL_INCR_WRAP
    invert    = 0x150A, // GL_INVERT
    keep      = 0x1E00, // GL_KEEP
    replace   = 0x1E01, // GL_REPLACE
    zero      = 0,      // GL_ZERO
}; // enum class stencil_op

} // namespace gl::group
