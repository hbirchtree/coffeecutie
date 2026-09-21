#pragma once

#include "common.h"

namespace gl::group {

// GetMultisamplePNameNV
enum class get_multisample_prop_nv : u32
{
    sample_position = 0x8E50, // GL_SAMPLE_POSITION
    programmable_sample_location_arb =
        0x9341,                   // GL_PROGRAMMABLE_SAMPLE_LOCATION_ARB
    sample_location_arb = 0x8E50, // GL_SAMPLE_LOCATION_ARB
}; // enum class get_multisample_prop_nv

} // namespace gl::group
