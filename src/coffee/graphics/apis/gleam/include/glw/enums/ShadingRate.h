#pragma once

#include "common.h"

namespace gl::group {

// ShadingRate
enum class shading_rate : u32
{
#ifdef GL_SHADING_RATE_1X1_PIXELS_EXT
    1x1_pixels_ext = GL_SHADING_RATE_1X1_PIXELS_EXT,
#endif
#ifdef GL_SHADING_RATE_1X2_PIXELS_EXT
    1x2_pixels_ext = GL_SHADING_RATE_1X2_PIXELS_EXT,
#endif
#ifdef GL_SHADING_RATE_1X4_PIXELS_EXT
    1x4_pixels_ext = GL_SHADING_RATE_1X4_PIXELS_EXT,
#endif
#ifdef GL_SHADING_RATE_2X1_PIXELS_EXT
    2x1_pixels_ext = GL_SHADING_RATE_2X1_PIXELS_EXT,
#endif
#ifdef GL_SHADING_RATE_2X2_PIXELS_EXT
    2x2_pixels_ext = GL_SHADING_RATE_2X2_PIXELS_EXT,
#endif
#ifdef GL_SHADING_RATE_2X4_PIXELS_EXT
    2x4_pixels_ext = GL_SHADING_RATE_2X4_PIXELS_EXT,
#endif
#ifdef GL_SHADING_RATE_4X1_PIXELS_EXT
    4x1_pixels_ext = GL_SHADING_RATE_4X1_PIXELS_EXT,
#endif
#ifdef GL_SHADING_RATE_4X2_PIXELS_EXT
    4x2_pixels_ext = GL_SHADING_RATE_4X2_PIXELS_EXT,
#endif
#ifdef GL_SHADING_RATE_4X4_PIXELS_EXT
    4x4_pixels_ext = GL_SHADING_RATE_4X4_PIXELS_EXT,
#endif
}; // enum class shading_rate

} // namespace gl::group
