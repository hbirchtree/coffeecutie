#pragma once

#include "common.h"

namespace gl::group {

// ConvolutionBorderModeEXT
enum class convolution_border_mode_ext : u32
{
    reduce     = 0x8016, // GL_REDUCE
    reduce_ext = 0x8016, // GL_REDUCE_EXT
}; // enum class convolution_border_mode_ext

} // namespace gl::group
