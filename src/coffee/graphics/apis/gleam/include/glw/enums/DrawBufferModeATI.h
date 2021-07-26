#pragma once

#include "common.h"

namespace gl::group {

// DrawBufferModeATI
enum class draw_buffer_mode_ati : ::libc_types::u32
{
#ifdef GL_COLOR_ATTACHMENT0_NV
    color_attachment0_nv = GL_COLOR_ATTACHMENT0_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT10_NV
    color_attachment10_nv = GL_COLOR_ATTACHMENT10_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT11_NV
    color_attachment11_nv = GL_COLOR_ATTACHMENT11_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT12_NV
    color_attachment12_nv = GL_COLOR_ATTACHMENT12_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT13_NV
    color_attachment13_nv = GL_COLOR_ATTACHMENT13_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT14_NV
    color_attachment14_nv = GL_COLOR_ATTACHMENT14_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT15_NV
    color_attachment15_nv = GL_COLOR_ATTACHMENT15_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT1_NV
    color_attachment1_nv = GL_COLOR_ATTACHMENT1_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT2_NV
    color_attachment2_nv = GL_COLOR_ATTACHMENT2_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT3_NV
    color_attachment3_nv = GL_COLOR_ATTACHMENT3_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT4_NV
    color_attachment4_nv = GL_COLOR_ATTACHMENT4_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT5_NV
    color_attachment5_nv = GL_COLOR_ATTACHMENT5_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT6_NV
    color_attachment6_nv = GL_COLOR_ATTACHMENT6_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT7_NV
    color_attachment7_nv = GL_COLOR_ATTACHMENT7_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT8_NV
    color_attachment8_nv = GL_COLOR_ATTACHMENT8_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT9_NV
    color_attachment9_nv = GL_COLOR_ATTACHMENT9_NV,
#endif
}; // enum class draw_buffer_mode_ati

} // namespace gl::group
