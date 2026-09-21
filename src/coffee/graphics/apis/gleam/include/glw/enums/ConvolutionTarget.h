#pragma once

#include "common.h"

namespace gl::group {

// ConvolutionTarget
enum class convolution_target : u32
{
    convolution_1d = 0x8010, // GL_CONVOLUTION_1D
    convolution_2d = 0x8011, // GL_CONVOLUTION_2D
}; // enum class convolution_target

} // namespace gl::group
