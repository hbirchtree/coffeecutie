#pragma once

#include "common.h"

namespace gl::group {

// ConvolutionTarget
enum class convolution_target : ::libc_types::u32
{
#ifdef GL_CONVOLUTION_1D
    convolution_1d = GL_CONVOLUTION_1D,
#endif
#ifdef GL_CONVOLUTION_2D
    convolution_2d = GL_CONVOLUTION_2D,
#endif
}; // enum class convolution_target

} // namespace gl::group
