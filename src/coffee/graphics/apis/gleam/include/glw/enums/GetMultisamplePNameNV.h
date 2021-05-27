#pragma once

#include "common.h"

namespace gl::groups {

// GetMultisamplePNameNV
enum class get_multisample_prop_nv : ::libc_types::u32 {
#ifdef GL_PROGRAMMABLE_SAMPLE_LOCATION_ARB
    programmable_sample_location_arb = GL_PROGRAMMABLE_SAMPLE_LOCATION_ARB,
#endif
#ifdef GL_SAMPLE_LOCATION_ARB
    sample_location_arb = GL_SAMPLE_LOCATION_ARB,
#endif
#ifdef GL_SAMPLE_POSITION
    sample_position = GL_SAMPLE_POSITION,
#endif
}; // enum class get_multisample_prop_nv

} // namespace gl::groups
