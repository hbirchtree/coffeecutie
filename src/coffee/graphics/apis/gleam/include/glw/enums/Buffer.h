#pragma once

#include "common.h"

namespace gl::group {

// Buffer
enum class buffer : u32
{
    color   = 0x1800, // GL_COLOR
    depth   = 0x1801, // GL_DEPTH
    stencil = 0x1802, // GL_STENCIL
}; // enum class buffer

} // namespace gl::group
