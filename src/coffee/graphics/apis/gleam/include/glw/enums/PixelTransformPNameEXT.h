#pragma once

#include "common.h"

namespace gl::group {

// PixelTransformPNameEXT
enum class pixel_transform_prop_ext : u32
{
    pixel_cubic_weight_ext = 0x8333, // GL_PIXEL_CUBIC_WEIGHT_EXT
    pixel_mag_filter_ext   = 0x8331, // GL_PIXEL_MAG_FILTER_EXT
    pixel_min_filter_ext   = 0x8332, // GL_PIXEL_MIN_FILTER_EXT
}; // enum class pixel_transform_prop_ext

} // namespace gl::group
