#pragma once

#include "common.h"

namespace gl::groups {

// SamplerParameterI
enum class sampler_parameter_i : ::libc_types::u32 {
#ifdef GL_TEXTURE_COMPARE_FUNC
    texture_compare_func = GL_TEXTURE_COMPARE_FUNC,
#endif
#ifdef GL_TEXTURE_COMPARE_MODE
    texture_compare_mode = GL_TEXTURE_COMPARE_MODE,
#endif
#ifdef GL_TEXTURE_MAG_FILTER
    texture_mag_filter = GL_TEXTURE_MAG_FILTER,
#endif
#ifdef GL_TEXTURE_MIN_FILTER
    texture_min_filter = GL_TEXTURE_MIN_FILTER,
#endif
#ifdef GL_TEXTURE_UNNORMALIZED_COORDINATES_ARM
    texture_unnormalized_coordinates_arm = GL_TEXTURE_UNNORMALIZED_COORDINATES_ARM,
#endif
#ifdef GL_TEXTURE_WRAP_R
    texture_wrap_r = GL_TEXTURE_WRAP_R,
#endif
#ifdef GL_TEXTURE_WRAP_S
    texture_wrap_s = GL_TEXTURE_WRAP_S,
#endif
#ifdef GL_TEXTURE_WRAP_T
    texture_wrap_t = GL_TEXTURE_WRAP_T,
#endif
}; // enum class sampler_parameter_i

} // namespace gl::groups
