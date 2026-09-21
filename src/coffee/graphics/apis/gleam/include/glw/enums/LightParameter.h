#pragma once

#include "common.h"

namespace gl::group {

// LightParameter
enum class light_parameter : u32
{
    ambient               = 0x1200, // GL_AMBIENT
    constant_attenuation  = 0x1207, // GL_CONSTANT_ATTENUATION
    diffuse               = 0x1201, // GL_DIFFUSE
    linear_attenuation    = 0x1208, // GL_LINEAR_ATTENUATION
    position              = 0x1203, // GL_POSITION
    quadratic_attenuation = 0x1209, // GL_QUADRATIC_ATTENUATION
    specular              = 0x1202, // GL_SPECULAR
    spot_cutoff           = 0x1206, // GL_SPOT_CUTOFF
    spot_direction        = 0x1204, // GL_SPOT_DIRECTION
    spot_exponent         = 0x1205, // GL_SPOT_EXPONENT
}; // enum class light_parameter

} // namespace gl::group
