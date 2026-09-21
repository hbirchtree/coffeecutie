#pragma once

#include "common.h"

namespace gl::group {

// BlendingFactor
enum class blending_factor : u32
{
    constant_alpha           = 0x8003, // GL_CONSTANT_ALPHA
    constant_color           = 0x8001, // GL_CONSTANT_COLOR
    dst_alpha                = 0x0304, // GL_DST_ALPHA
    dst_color                = 0x0306, // GL_DST_COLOR
    one                      = 1,      // GL_ONE
    one_minus_constant_alpha = 0x8004, // GL_ONE_MINUS_CONSTANT_ALPHA
    one_minus_constant_color = 0x8002, // GL_ONE_MINUS_CONSTANT_COLOR
    one_minus_dst_alpha      = 0x0305, // GL_ONE_MINUS_DST_ALPHA
    one_minus_dst_color      = 0x0307, // GL_ONE_MINUS_DST_COLOR
    one_minus_src1_alpha     = 0x88FB, // GL_ONE_MINUS_SRC1_ALPHA
    one_minus_src1_color     = 0x88FA, // GL_ONE_MINUS_SRC1_COLOR
    one_minus_src_alpha      = 0x0303, // GL_ONE_MINUS_SRC_ALPHA
    one_minus_src_color      = 0x0301, // GL_ONE_MINUS_SRC_COLOR
    src1_alpha               = 0x8589, // GL_SRC1_ALPHA
    src1_color               = 0x88F9, // GL_SRC1_COLOR
    src_alpha                = 0x0302, // GL_SRC_ALPHA
    src_alpha_saturate       = 0x0308, // GL_SRC_ALPHA_SATURATE
    src_color                = 0x0300, // GL_SRC_COLOR
    zero                     = 0,      // GL_ZERO
}; // enum class blending_factor

} // namespace gl::group
