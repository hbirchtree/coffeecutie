#pragma once

#include "common.h"

namespace gl::group {

// MapTextureFormatINTEL
enum class map_texture_format_intel : u32
{
    layout_default_intel           = 0, // GL_LAYOUT_DEFAULT_INTEL
    layout_linear_cpu_cached_intel = 2, // GL_LAYOUT_LINEAR_CPU_CACHED_INTEL
    layout_linear_intel            = 1, // GL_LAYOUT_LINEAR_INTEL
}; // enum class map_texture_format_intel

} // namespace gl::group
