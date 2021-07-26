#pragma once

#include "common.h"

namespace gl::group {

// PixelTransformTargetEXT
enum class pixel_transform_target_ext : ::libc_types::u32
{
#ifdef GL_PIXEL_TRANSFORM_2D_EXT
    pixel_transform_2d_ext = GL_PIXEL_TRANSFORM_2D_EXT,
#endif
}; // enum class pixel_transform_target_ext

} // namespace gl::group
