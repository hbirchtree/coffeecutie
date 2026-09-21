#pragma once

#include "common.h"

namespace gl::group {

// SamplerParameterF
enum class sampler_parameter_f : u32
{
    texture_border_color       = 0x1004, // GL_TEXTURE_BORDER_COLOR
    texture_lod_bias           = 0x8501, // GL_TEXTURE_LOD_BIAS
    texture_max_anisotropy     = 0x84FE, // GL_TEXTURE_MAX_ANISOTROPY
    texture_max_lod            = 0x813B, // GL_TEXTURE_MAX_LOD
    texture_min_lod            = 0x813A, // GL_TEXTURE_MIN_LOD
    texture_lod_bias_ext       = 0x8501, // GL_TEXTURE_LOD_BIAS_EXT
    texture_max_anisotropy_ext = 0x84FE, // GL_TEXTURE_MAX_ANISOTROPY_EXT
    texture_unnormalized_coordinates_arm =
        0x8F6A, // GL_TEXTURE_UNNORMALIZED_COORDINATES_ARM
}; // enum class sampler_parameter_f

} // namespace gl::group
