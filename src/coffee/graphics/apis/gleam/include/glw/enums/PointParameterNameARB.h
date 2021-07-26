#pragma once

#include "common.h"

namespace gl::group {

// PointParameterNameARB
enum class point_parameter_name_arb : ::libc_types::u32
{
#ifdef GL_POINT_FADE_THRESHOLD_SIZE
    point_fade_threshold_size = GL_POINT_FADE_THRESHOLD_SIZE,
#endif
#ifdef GL_POINT_FADE_THRESHOLD_SIZE_EXT
    point_fade_threshold_size_ext = GL_POINT_FADE_THRESHOLD_SIZE_EXT,
#endif
#ifdef GL_POINT_SIZE_MAX_EXT
    point_size_max_ext = GL_POINT_SIZE_MAX_EXT,
#endif
#ifdef GL_POINT_SIZE_MIN_EXT
    point_size_min_ext = GL_POINT_SIZE_MIN_EXT,
#endif
}; // enum class point_parameter_name_arb

} // namespace gl::group
