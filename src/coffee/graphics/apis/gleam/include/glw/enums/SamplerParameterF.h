#pragma once

#include "common.h"

namespace gl::group {

// SamplerParameterF
enum class sampler_parameter_f : ::libc_types::u32
{
#ifdef GL_TEXTURE_BORDER_COLOR
    texture_border_color = GL_TEXTURE_BORDER_COLOR,
#endif
#ifdef GL_TEXTURE_LOD_BIAS
    texture_lod_bias = GL_TEXTURE_LOD_BIAS,
#endif
#ifdef GL_TEXTURE_LOD_BIAS_EXT
    texture_lod_bias_ext = GL_TEXTURE_LOD_BIAS_EXT,
#endif
#ifdef GL_TEXTURE_MAX_ANISOTROPY
    texture_max_anisotropy = GL_TEXTURE_MAX_ANISOTROPY,
#endif
#ifdef GL_TEXTURE_MAX_ANISOTROPY_EXT
    texture_max_anisotropy_ext = GL_TEXTURE_MAX_ANISOTROPY_EXT,
#endif
#ifdef GL_TEXTURE_MAX_LOD
    texture_max_lod = GL_TEXTURE_MAX_LOD,
#endif
#ifdef GL_TEXTURE_MIN_LOD
    texture_min_lod = GL_TEXTURE_MIN_LOD,
#endif
#ifdef GL_TEXTURE_UNNORMALIZED_COORDINATES_ARM
    texture_unnormalized_coordinates_arm =
        GL_TEXTURE_UNNORMALIZED_COORDINATES_ARM,
#endif
}; // enum class sampler_parameter_f

} // namespace gl::group
