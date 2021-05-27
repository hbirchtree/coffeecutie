#pragma once

#include "common.h"

namespace gl::groups {

// BlendingFactor
enum class blending_factor : ::libc_types::u32 {
#ifdef GL_CONSTANT_ALPHA
    constant_alpha = GL_CONSTANT_ALPHA,
#endif
#ifdef GL_CONSTANT_COLOR
    constant_color = GL_CONSTANT_COLOR,
#endif
#ifdef GL_DST_ALPHA
    dst_alpha = GL_DST_ALPHA,
#endif
#ifdef GL_DST_COLOR
    dst_color = GL_DST_COLOR,
#endif
#ifdef GL_ONE
    one = GL_ONE,
#endif
#ifdef GL_ONE_MINUS_CONSTANT_ALPHA
    one_minus_constant_alpha = GL_ONE_MINUS_CONSTANT_ALPHA,
#endif
#ifdef GL_ONE_MINUS_CONSTANT_COLOR
    one_minus_constant_color = GL_ONE_MINUS_CONSTANT_COLOR,
#endif
#ifdef GL_ONE_MINUS_DST_ALPHA
    one_minus_dst_alpha = GL_ONE_MINUS_DST_ALPHA,
#endif
#ifdef GL_ONE_MINUS_DST_COLOR
    one_minus_dst_color = GL_ONE_MINUS_DST_COLOR,
#endif
#ifdef GL_ONE_MINUS_SRC1_ALPHA
    one_minus_src1_alpha = GL_ONE_MINUS_SRC1_ALPHA,
#endif
#ifdef GL_ONE_MINUS_SRC1_COLOR
    one_minus_src1_color = GL_ONE_MINUS_SRC1_COLOR,
#endif
#ifdef GL_ONE_MINUS_SRC_ALPHA
    one_minus_src_alpha = GL_ONE_MINUS_SRC_ALPHA,
#endif
#ifdef GL_ONE_MINUS_SRC_COLOR
    one_minus_src_color = GL_ONE_MINUS_SRC_COLOR,
#endif
#ifdef GL_SRC1_ALPHA
    src1_alpha = GL_SRC1_ALPHA,
#endif
#ifdef GL_SRC1_COLOR
    src1_color = GL_SRC1_COLOR,
#endif
#ifdef GL_SRC_ALPHA
    src_alpha = GL_SRC_ALPHA,
#endif
#ifdef GL_SRC_ALPHA_SATURATE
    src_alpha_saturate = GL_SRC_ALPHA_SATURATE,
#endif
#ifdef GL_SRC_COLOR
    src_color = GL_SRC_COLOR,
#endif
#ifdef GL_ZERO
    zero = GL_ZERO,
#endif
}; // enum class blending_factor

} // namespace gl::groups
