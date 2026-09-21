#pragma once

#include "common.h"

namespace gl::group {

// CombinerRegisterNV
enum class combiner_register_nv : u32
{
    discard_nv         = 0x8530, // GL_DISCARD_NV
    primary_color_nv   = 0x852C, // GL_PRIMARY_COLOR_NV
    secondary_color_nv = 0x852D, // GL_SECONDARY_COLOR_NV
    spare0_nv          = 0x852E, // GL_SPARE0_NV
    spare1_nv          = 0x852F, // GL_SPARE1_NV
    texture0_arb       = 0x84C0, // GL_TEXTURE0_ARB
    texture1_arb       = 0x84C1, // GL_TEXTURE1_ARB
}; // enum class combiner_register_nv

} // namespace gl::group
