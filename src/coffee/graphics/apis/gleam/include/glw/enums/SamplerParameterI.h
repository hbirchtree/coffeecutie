#pragma once

#include "common.h"

namespace gl::group {

// SamplerParameterI
enum class sampler_parameter_i : u32
{
    texture_compare_func = 0x884D, // GL_TEXTURE_COMPARE_FUNC
    texture_compare_mode = 0x884C, // GL_TEXTURE_COMPARE_MODE
    texture_mag_filter   = 0x2800, // GL_TEXTURE_MAG_FILTER
    texture_min_filter   = 0x2801, // GL_TEXTURE_MIN_FILTER
    texture_wrap_r       = 0x8072, // GL_TEXTURE_WRAP_R
    texture_wrap_s       = 0x2802, // GL_TEXTURE_WRAP_S
    texture_wrap_t       = 0x2803, // GL_TEXTURE_WRAP_T
    texture_unnormalized_coordinates_arm =
        0x8F6A, // GL_TEXTURE_UNNORMALIZED_COORDINATES_ARM
}; // enum class sampler_parameter_i

} // namespace gl::group
