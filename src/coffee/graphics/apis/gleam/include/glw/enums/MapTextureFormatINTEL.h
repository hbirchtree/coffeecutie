#pragma once

#include "common.h"

namespace gl::group {

// MapTextureFormatINTEL
enum class map_texture_format_intel : u32
{
#ifdef GL_LAYOUT_DEFAULT_INTEL
    layout_default_intel = GL_LAYOUT_DEFAULT_INTEL,
#endif
#ifdef GL_LAYOUT_LINEAR_CPU_CACHED_INTEL
    layout_linear_cpu_cached_intel = GL_LAYOUT_LINEAR_CPU_CACHED_INTEL,
#endif
#ifdef GL_LAYOUT_LINEAR_INTEL
    layout_linear_intel = GL_LAYOUT_LINEAR_INTEL,
#endif
}; // enum class map_texture_format_intel

} // namespace gl::group
