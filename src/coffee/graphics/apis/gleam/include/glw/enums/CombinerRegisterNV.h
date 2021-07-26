#pragma once

#include "common.h"

namespace gl::group {

// CombinerRegisterNV
enum class combiner_register_nv : ::libc_types::u32
{
#ifdef GL_DISCARD_NV
    discard_nv = GL_DISCARD_NV,
#endif
#ifdef GL_PRIMARY_COLOR_NV
    primary_color_nv = GL_PRIMARY_COLOR_NV,
#endif
#ifdef GL_SECONDARY_COLOR_NV
    secondary_color_nv = GL_SECONDARY_COLOR_NV,
#endif
#ifdef GL_SPARE0_NV
    spare0_nv = GL_SPARE0_NV,
#endif
#ifdef GL_SPARE1_NV
    spare1_nv = GL_SPARE1_NV,
#endif
#ifdef GL_TEXTURE0_ARB
    texture0_arb = GL_TEXTURE0_ARB,
#endif
#ifdef GL_TEXTURE1_ARB
    texture1_arb = GL_TEXTURE1_ARB,
#endif
}; // enum class combiner_register_nv

} // namespace gl::group
