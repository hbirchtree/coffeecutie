#pragma once

#include "common.h"

namespace gl::group {

// ConvolutionTargetEXT
enum class convolution_target_ext : u32
{
#ifdef GL_CONVOLUTION_1D
    convolution_1d = GL_CONVOLUTION_1D,
#endif
#ifdef GL_CONVOLUTION_1D_EXT
    convolution_1d_ext = GL_CONVOLUTION_1D_EXT,
#endif
#ifdef GL_CONVOLUTION_2D
    convolution_2d = GL_CONVOLUTION_2D,
#endif
#ifdef GL_CONVOLUTION_2D_EXT
    convolution_2d_ext = GL_CONVOLUTION_2D_EXT,
#endif
}; // enum class convolution_target_ext

} // namespace gl::group
