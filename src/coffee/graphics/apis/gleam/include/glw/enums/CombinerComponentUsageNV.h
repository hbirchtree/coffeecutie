#pragma once

#include "common.h"

namespace gl::group {

// CombinerComponentUsageNV
enum class combiner_component_usage_nv : ::libc_types::u32
{
#ifdef GL_ALPHA
    alpha = GL_ALPHA,
#endif
#ifdef GL_BLUE
    blue = GL_BLUE,
#endif
#ifdef GL_RGB
    rgb = GL_RGB,
#endif
}; // enum class combiner_component_usage_nv

} // namespace gl::group
