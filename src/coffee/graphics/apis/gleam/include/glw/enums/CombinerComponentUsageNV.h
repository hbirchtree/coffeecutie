#pragma once

#include "common.h"

namespace gl::group {

// CombinerComponentUsageNV
enum class combiner_component_usage_nv : u32
{
    alpha = 0x1906, // GL_ALPHA
    blue  = 0x1905, // GL_BLUE
    rgb   = 0x1907, // GL_RGB
}; // enum class combiner_component_usage_nv

} // namespace gl::group
