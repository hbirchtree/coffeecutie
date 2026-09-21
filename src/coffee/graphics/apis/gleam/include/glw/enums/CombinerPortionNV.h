#pragma once

#include "common.h"

namespace gl::group {

// CombinerPortionNV
enum class combiner_portion_nv : u32
{
    alpha = 0x1906, // GL_ALPHA
    rgb   = 0x1907, // GL_RGB
}; // enum class combiner_portion_nv

} // namespace gl::group
