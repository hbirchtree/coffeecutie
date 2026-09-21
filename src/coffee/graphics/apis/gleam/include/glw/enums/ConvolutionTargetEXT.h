#pragma once

#include "common.h"

namespace gl::group {

// ConvolutionTargetEXT
enum class convolution_target_ext : u32
{
    convolution_1d     = 0x8010, // GL_CONVOLUTION_1D
    convolution_2d     = 0x8011, // GL_CONVOLUTION_2D
    convolution_1d_ext = 0x8010, // GL_CONVOLUTION_1D_EXT
    convolution_2d_ext = 0x8011, // GL_CONVOLUTION_2D_EXT
}; // enum class convolution_target_ext

} // namespace gl::group
