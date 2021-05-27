#pragma once

#include "common.h"

namespace gl::groups {

// LightParameter
enum class light_parameter : ::libc_types::u32 {
#ifdef GL_CONSTANT_ATTENUATION
    constant_attenuation = GL_CONSTANT_ATTENUATION,
#endif
#ifdef GL_LINEAR_ATTENUATION
    linear_attenuation = GL_LINEAR_ATTENUATION,
#endif
#ifdef GL_POSITION
    position = GL_POSITION,
#endif
#ifdef GL_QUADRATIC_ATTENUATION
    quadratic_attenuation = GL_QUADRATIC_ATTENUATION,
#endif
#ifdef GL_SPOT_CUTOFF
    spot_cutoff = GL_SPOT_CUTOFF,
#endif
#ifdef GL_SPOT_DIRECTION
    spot_direction = GL_SPOT_DIRECTION,
#endif
#ifdef GL_SPOT_EXPONENT
    spot_exponent = GL_SPOT_EXPONENT,
#endif
}; // enum class light_parameter

} // namespace gl::groups
