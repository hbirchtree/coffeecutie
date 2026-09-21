#pragma once

#include "common.h"

namespace gl::group {

// PointParameterNameARB
enum class point_parameter_name_arb : u32
{
    point_distance_attenuation = 0x8129, // GL_POINT_DISTANCE_ATTENUATION
    point_fade_threshold_size  = 0x8128, // GL_POINT_FADE_THRESHOLD_SIZE
    point_size_max             = 0x8127, // GL_POINT_SIZE_MAX
    point_size_min             = 0x8126, // GL_POINT_SIZE_MIN
    distance_attenuation_ext   = 0x8129, // GL_DISTANCE_ATTENUATION_EXT
    distance_attenuation_sgis  = 0x8129, // GL_DISTANCE_ATTENUATION_SGIS
    point_distance_attenuation_arb =
        0x8129,                             // GL_POINT_DISTANCE_ATTENUATION_ARB
    point_fade_threshold_size_arb = 0x8128, // GL_POINT_FADE_THRESHOLD_SIZE_ARB
    point_fade_threshold_size_ext = 0x8128, // GL_POINT_FADE_THRESHOLD_SIZE_EXT
    point_fade_threshold_size_sgis =
        0x8128,                   // GL_POINT_FADE_THRESHOLD_SIZE_SGIS
    point_size_max_arb  = 0x8127, // GL_POINT_SIZE_MAX_ARB
    point_size_max_ext  = 0x8127, // GL_POINT_SIZE_MAX_EXT
    point_size_max_sgis = 0x8127, // GL_POINT_SIZE_MAX_SGIS
    point_size_min_arb  = 0x8126, // GL_POINT_SIZE_MIN_ARB
    point_size_min_ext  = 0x8126, // GL_POINT_SIZE_MIN_EXT
    point_size_min_sgis = 0x8126, // GL_POINT_SIZE_MIN_SGIS
}; // enum class point_parameter_name_arb

} // namespace gl::group
