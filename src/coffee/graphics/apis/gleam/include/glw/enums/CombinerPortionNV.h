#pragma once

#include "common.h"

namespace gl::groups {

// CombinerPortionNV
enum class combiner_portion_nv : ::libc_types::u32 {
#ifdef GL_ALPHA
    alpha = GL_ALPHA,
#endif
#ifdef GL_RGB
    rgb = GL_RGB,
#endif
}; // enum class combiner_portion_nv

} // namespace gl::groups
