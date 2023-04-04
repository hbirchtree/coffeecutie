#pragma once

#include "common.h"

namespace gl::group {

// PixelTransformPNameEXT
enum class pixel_transform_prop_ext : u32
{
#ifdef GL_PIXEL_CUBIC_WEIGHT_EXT
    pixel_cubic_weight_ext = GL_PIXEL_CUBIC_WEIGHT_EXT,
#endif
#ifdef GL_PIXEL_MAG_FILTER_EXT
    pixel_mag_filter_ext = GL_PIXEL_MAG_FILTER_EXT,
#endif
#ifdef GL_PIXEL_MIN_FILTER_EXT
    pixel_min_filter_ext = GL_PIXEL_MIN_FILTER_EXT,
#endif
}; // enum class pixel_transform_prop_ext

} // namespace gl::group
