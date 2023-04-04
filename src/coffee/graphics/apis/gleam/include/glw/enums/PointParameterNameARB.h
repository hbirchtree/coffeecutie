#pragma once

#include "common.h"

namespace gl::group {

// PointParameterNameARB
enum class point_parameter_name_arb : u32
{
#ifdef GL_DISTANCE_ATTENUATION_EXT
    distance_attenuation_ext = GL_DISTANCE_ATTENUATION_EXT,
#endif
#ifdef GL_DISTANCE_ATTENUATION_SGIS
    distance_attenuation_sgis = GL_DISTANCE_ATTENUATION_SGIS,
#endif
#ifdef GL_POINT_DISTANCE_ATTENUATION
    point_distance_attenuation = GL_POINT_DISTANCE_ATTENUATION,
#endif
#ifdef GL_POINT_DISTANCE_ATTENUATION_ARB
    point_distance_attenuation_arb = GL_POINT_DISTANCE_ATTENUATION_ARB,
#endif
#ifdef GL_POINT_FADE_THRESHOLD_SIZE
    point_fade_threshold_size = GL_POINT_FADE_THRESHOLD_SIZE,
#endif
#ifdef GL_POINT_FADE_THRESHOLD_SIZE_ARB
    point_fade_threshold_size_arb = GL_POINT_FADE_THRESHOLD_SIZE_ARB,
#endif
#ifdef GL_POINT_FADE_THRESHOLD_SIZE_EXT
    point_fade_threshold_size_ext = GL_POINT_FADE_THRESHOLD_SIZE_EXT,
#endif
#ifdef GL_POINT_FADE_THRESHOLD_SIZE_SGIS
    point_fade_threshold_size_sgis = GL_POINT_FADE_THRESHOLD_SIZE_SGIS,
#endif
#ifdef GL_POINT_SIZE_MAX
    point_size_max = GL_POINT_SIZE_MAX,
#endif
#ifdef GL_POINT_SIZE_MAX_ARB
    point_size_max_arb = GL_POINT_SIZE_MAX_ARB,
#endif
#ifdef GL_POINT_SIZE_MAX_EXT
    point_size_max_ext = GL_POINT_SIZE_MAX_EXT,
#endif
#ifdef GL_POINT_SIZE_MAX_SGIS
    point_size_max_sgis = GL_POINT_SIZE_MAX_SGIS,
#endif
#ifdef GL_POINT_SIZE_MIN
    point_size_min = GL_POINT_SIZE_MIN,
#endif
#ifdef GL_POINT_SIZE_MIN_ARB
    point_size_min_arb = GL_POINT_SIZE_MIN_ARB,
#endif
#ifdef GL_POINT_SIZE_MIN_EXT
    point_size_min_ext = GL_POINT_SIZE_MIN_EXT,
#endif
#ifdef GL_POINT_SIZE_MIN_SGIS
    point_size_min_sgis = GL_POINT_SIZE_MIN_SGIS,
#endif
}; // enum class point_parameter_name_arb

} // namespace gl::group
