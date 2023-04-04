#pragma once

#include "common.h"

namespace gl::group {

// ConvolutionBorderModeEXT
enum class convolution_border_mode_ext : u32
{
#ifdef GL_REDUCE
    reduce = GL_REDUCE,
#endif
#ifdef GL_REDUCE_EXT
    reduce_ext = GL_REDUCE_EXT,
#endif
}; // enum class convolution_border_mode_ext

} // namespace gl::group
