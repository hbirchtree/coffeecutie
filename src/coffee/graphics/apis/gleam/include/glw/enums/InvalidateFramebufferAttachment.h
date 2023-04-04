#pragma once

#include "common.h"

namespace gl::group {

// InvalidateFramebufferAttachment
enum class invalidate_framebuffer_attachment : u32
{
#ifdef GL_COLOR
    color = GL_COLOR,
#endif
#ifdef GL_COLOR_ATTACHMENT0
    color_attachment0 = GL_COLOR_ATTACHMENT0,
#endif
#ifdef GL_COLOR_ATTACHMENT0_EXT
    color_attachment0_ext = GL_COLOR_ATTACHMENT0_EXT,
#endif
#ifdef GL_COLOR_ATTACHMENT0_NV
    color_attachment0_nv = GL_COLOR_ATTACHMENT0_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT0_OES
    color_attachment0_oes = GL_COLOR_ATTACHMENT0_OES,
#endif
#ifdef GL_COLOR_ATTACHMENT1
    color_attachment1 = GL_COLOR_ATTACHMENT1,
#endif
#ifdef GL_COLOR_ATTACHMENT10
    color_attachment10 = GL_COLOR_ATTACHMENT10,
#endif
#ifdef GL_COLOR_ATTACHMENT10_EXT
    color_attachment10_ext = GL_COLOR_ATTACHMENT10_EXT,
#endif
#ifdef GL_COLOR_ATTACHMENT10_NV
    color_attachment10_nv = GL_COLOR_ATTACHMENT10_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT11
    color_attachment11 = GL_COLOR_ATTACHMENT11,
#endif
#ifdef GL_COLOR_ATTACHMENT11_EXT
    color_attachment11_ext = GL_COLOR_ATTACHMENT11_EXT,
#endif
#ifdef GL_COLOR_ATTACHMENT11_NV
    color_attachment11_nv = GL_COLOR_ATTACHMENT11_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT12
    color_attachment12 = GL_COLOR_ATTACHMENT12,
#endif
#ifdef GL_COLOR_ATTACHMENT12_EXT
    color_attachment12_ext = GL_COLOR_ATTACHMENT12_EXT,
#endif
#ifdef GL_COLOR_ATTACHMENT12_NV
    color_attachment12_nv = GL_COLOR_ATTACHMENT12_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT13
    color_attachment13 = GL_COLOR_ATTACHMENT13,
#endif
#ifdef GL_COLOR_ATTACHMENT13_EXT
    color_attachment13_ext = GL_COLOR_ATTACHMENT13_EXT,
#endif
#ifdef GL_COLOR_ATTACHMENT13_NV
    color_attachment13_nv = GL_COLOR_ATTACHMENT13_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT14
    color_attachment14 = GL_COLOR_ATTACHMENT14,
#endif
#ifdef GL_COLOR_ATTACHMENT14_EXT
    color_attachment14_ext = GL_COLOR_ATTACHMENT14_EXT,
#endif
#ifdef GL_COLOR_ATTACHMENT14_NV
    color_attachment14_nv = GL_COLOR_ATTACHMENT14_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT15
    color_attachment15 = GL_COLOR_ATTACHMENT15,
#endif
#ifdef GL_COLOR_ATTACHMENT15_EXT
    color_attachment15_ext = GL_COLOR_ATTACHMENT15_EXT,
#endif
#ifdef GL_COLOR_ATTACHMENT15_NV
    color_attachment15_nv = GL_COLOR_ATTACHMENT15_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT16
    color_attachment16 = GL_COLOR_ATTACHMENT16,
#endif
#ifdef GL_COLOR_ATTACHMENT17
    color_attachment17 = GL_COLOR_ATTACHMENT17,
#endif
#ifdef GL_COLOR_ATTACHMENT18
    color_attachment18 = GL_COLOR_ATTACHMENT18,
#endif
#ifdef GL_COLOR_ATTACHMENT19
    color_attachment19 = GL_COLOR_ATTACHMENT19,
#endif
#ifdef GL_COLOR_ATTACHMENT1_EXT
    color_attachment1_ext = GL_COLOR_ATTACHMENT1_EXT,
#endif
#ifdef GL_COLOR_ATTACHMENT1_NV
    color_attachment1_nv = GL_COLOR_ATTACHMENT1_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT2
    color_attachment2 = GL_COLOR_ATTACHMENT2,
#endif
#ifdef GL_COLOR_ATTACHMENT20
    color_attachment20 = GL_COLOR_ATTACHMENT20,
#endif
#ifdef GL_COLOR_ATTACHMENT21
    color_attachment21 = GL_COLOR_ATTACHMENT21,
#endif
#ifdef GL_COLOR_ATTACHMENT22
    color_attachment22 = GL_COLOR_ATTACHMENT22,
#endif
#ifdef GL_COLOR_ATTACHMENT23
    color_attachment23 = GL_COLOR_ATTACHMENT23,
#endif
#ifdef GL_COLOR_ATTACHMENT24
    color_attachment24 = GL_COLOR_ATTACHMENT24,
#endif
#ifdef GL_COLOR_ATTACHMENT25
    color_attachment25 = GL_COLOR_ATTACHMENT25,
#endif
#ifdef GL_COLOR_ATTACHMENT26
    color_attachment26 = GL_COLOR_ATTACHMENT26,
#endif
#ifdef GL_COLOR_ATTACHMENT27
    color_attachment27 = GL_COLOR_ATTACHMENT27,
#endif
#ifdef GL_COLOR_ATTACHMENT28
    color_attachment28 = GL_COLOR_ATTACHMENT28,
#endif
#ifdef GL_COLOR_ATTACHMENT29
    color_attachment29 = GL_COLOR_ATTACHMENT29,
#endif
#ifdef GL_COLOR_ATTACHMENT2_EXT
    color_attachment2_ext = GL_COLOR_ATTACHMENT2_EXT,
#endif
#ifdef GL_COLOR_ATTACHMENT2_NV
    color_attachment2_nv = GL_COLOR_ATTACHMENT2_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT3
    color_attachment3 = GL_COLOR_ATTACHMENT3,
#endif
#ifdef GL_COLOR_ATTACHMENT30
    color_attachment30 = GL_COLOR_ATTACHMENT30,
#endif
#ifdef GL_COLOR_ATTACHMENT31
    color_attachment31 = GL_COLOR_ATTACHMENT31,
#endif
#ifdef GL_COLOR_ATTACHMENT3_EXT
    color_attachment3_ext = GL_COLOR_ATTACHMENT3_EXT,
#endif
#ifdef GL_COLOR_ATTACHMENT3_NV
    color_attachment3_nv = GL_COLOR_ATTACHMENT3_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT4
    color_attachment4 = GL_COLOR_ATTACHMENT4,
#endif
#ifdef GL_COLOR_ATTACHMENT4_EXT
    color_attachment4_ext = GL_COLOR_ATTACHMENT4_EXT,
#endif
#ifdef GL_COLOR_ATTACHMENT4_NV
    color_attachment4_nv = GL_COLOR_ATTACHMENT4_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT5
    color_attachment5 = GL_COLOR_ATTACHMENT5,
#endif
#ifdef GL_COLOR_ATTACHMENT5_EXT
    color_attachment5_ext = GL_COLOR_ATTACHMENT5_EXT,
#endif
#ifdef GL_COLOR_ATTACHMENT5_NV
    color_attachment5_nv = GL_COLOR_ATTACHMENT5_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT6
    color_attachment6 = GL_COLOR_ATTACHMENT6,
#endif
#ifdef GL_COLOR_ATTACHMENT6_EXT
    color_attachment6_ext = GL_COLOR_ATTACHMENT6_EXT,
#endif
#ifdef GL_COLOR_ATTACHMENT6_NV
    color_attachment6_nv = GL_COLOR_ATTACHMENT6_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT7
    color_attachment7 = GL_COLOR_ATTACHMENT7,
#endif
#ifdef GL_COLOR_ATTACHMENT7_EXT
    color_attachment7_ext = GL_COLOR_ATTACHMENT7_EXT,
#endif
#ifdef GL_COLOR_ATTACHMENT7_NV
    color_attachment7_nv = GL_COLOR_ATTACHMENT7_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT8
    color_attachment8 = GL_COLOR_ATTACHMENT8,
#endif
#ifdef GL_COLOR_ATTACHMENT8_EXT
    color_attachment8_ext = GL_COLOR_ATTACHMENT8_EXT,
#endif
#ifdef GL_COLOR_ATTACHMENT8_NV
    color_attachment8_nv = GL_COLOR_ATTACHMENT8_NV,
#endif
#ifdef GL_COLOR_ATTACHMENT9
    color_attachment9 = GL_COLOR_ATTACHMENT9,
#endif
#ifdef GL_COLOR_ATTACHMENT9_EXT
    color_attachment9_ext = GL_COLOR_ATTACHMENT9_EXT,
#endif
#ifdef GL_COLOR_ATTACHMENT9_NV
    color_attachment9_nv = GL_COLOR_ATTACHMENT9_NV,
#endif
#ifdef GL_COLOR_EXT
    color_ext = GL_COLOR_EXT,
#endif
#ifdef GL_DEPTH
    depth = GL_DEPTH,
#endif
#ifdef GL_DEPTH_ATTACHMENT
    depth_attachment = GL_DEPTH_ATTACHMENT,
#endif
#ifdef GL_DEPTH_ATTACHMENT_EXT
    depth_attachment_ext = GL_DEPTH_ATTACHMENT_EXT,
#endif
#ifdef GL_DEPTH_ATTACHMENT_OES
    depth_attachment_oes = GL_DEPTH_ATTACHMENT_OES,
#endif
#ifdef GL_DEPTH_EXT
    depth_ext = GL_DEPTH_EXT,
#endif
#ifdef GL_DEPTH_STENCIL_ATTACHMENT
    depth_stencil_attachment = GL_DEPTH_STENCIL_ATTACHMENT,
#endif
#ifdef GL_STENCIL
    stencil = GL_STENCIL,
#endif
#ifdef GL_STENCIL_ATTACHMENT_EXT
    stencil_attachment_ext = GL_STENCIL_ATTACHMENT_EXT,
#endif
#ifdef GL_STENCIL_ATTACHMENT_OES
    stencil_attachment_oes = GL_STENCIL_ATTACHMENT_OES,
#endif
#ifdef GL_STENCIL_EXT
    stencil_ext = GL_STENCIL_EXT,
#endif
}; // enum class invalidate_framebuffer_attachment

} // namespace gl::group
