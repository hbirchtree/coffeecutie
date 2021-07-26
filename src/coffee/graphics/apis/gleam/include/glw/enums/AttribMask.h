#pragma once

#include "common.h"

namespace gl::group {

// AttribMask
enum class attrib_mask : ::libc_types::u32
{
#ifdef GL_ACCUM_BUFFER_BIT
    accum_buffer_bit = GL_ACCUM_BUFFER_BIT,
#endif
#ifdef GL_ALL_ATTRIB_BITS
    all_attrib_bits = GL_ALL_ATTRIB_BITS,
#endif
#ifdef GL_COLOR_BUFFER_BIT
    color_buffer_bit = GL_COLOR_BUFFER_BIT,
#endif
#ifdef GL_CURRENT_BIT
    current_bit = GL_CURRENT_BIT,
#endif
#ifdef GL_DEPTH_BUFFER_BIT
    depth_buffer_bit = GL_DEPTH_BUFFER_BIT,
#endif
#ifdef GL_ENABLE_BIT
    enable_bit = GL_ENABLE_BIT,
#endif
#ifdef GL_EVAL_BIT
    eval_bit = GL_EVAL_BIT,
#endif
#ifdef GL_FOG_BIT
    fog_bit = GL_FOG_BIT,
#endif
#ifdef GL_HINT_BIT
    hint_bit = GL_HINT_BIT,
#endif
#ifdef GL_LIGHTING_BIT
    lighting_bit = GL_LIGHTING_BIT,
#endif
#ifdef GL_LINE_BIT
    line_bit = GL_LINE_BIT,
#endif
#ifdef GL_LIST_BIT
    list_bit = GL_LIST_BIT,
#endif
#ifdef GL_MULTISAMPLE_BIT
    multisample_bit = GL_MULTISAMPLE_BIT,
#endif
#ifdef GL_MULTISAMPLE_BIT_3DFX
    multisample_bit_3dfx = GL_MULTISAMPLE_BIT_3DFX,
#endif
#ifdef GL_MULTISAMPLE_BIT_ARB
    multisample_bit_arb = GL_MULTISAMPLE_BIT_ARB,
#endif
#ifdef GL_MULTISAMPLE_BIT_EXT
    multisample_bit_ext = GL_MULTISAMPLE_BIT_EXT,
#endif
#ifdef GL_PIXEL_MODE_BIT
    pixel_mode_bit = GL_PIXEL_MODE_BIT,
#endif
#ifdef GL_POINT_BIT
    point_bit = GL_POINT_BIT,
#endif
#ifdef GL_POLYGON_BIT
    polygon_bit = GL_POLYGON_BIT,
#endif
#ifdef GL_POLYGON_STIPPLE_BIT
    polygon_stipple_bit = GL_POLYGON_STIPPLE_BIT,
#endif
#ifdef GL_SCISSOR_BIT
    scissor_bit = GL_SCISSOR_BIT,
#endif
#ifdef GL_STENCIL_BUFFER_BIT
    stencil_buffer_bit = GL_STENCIL_BUFFER_BIT,
#endif
#ifdef GL_TEXTURE_BIT
    texture_bit = GL_TEXTURE_BIT,
#endif
#ifdef GL_TRANSFORM_BIT
    transform_bit = GL_TRANSFORM_BIT,
#endif
#ifdef GL_VIEWPORT_BIT
    viewport_bit = GL_VIEWPORT_BIT,
#endif
}; // enum class attrib_mask
C_FLAGS(attrib_mask, ::libc_types::u32);

} // namespace gl::group
