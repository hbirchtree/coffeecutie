#pragma once

#include "common.h"

namespace gl::group {

// AttribMask
enum class attrib_mask : u32
{
    accum_buffer_bit     = 0x00000200, // GL_ACCUM_BUFFER_BIT
    all_attrib_bits      = 0xFFFFFFFF, // GL_ALL_ATTRIB_BITS
    color_buffer_bit     = 0x00004000, // GL_COLOR_BUFFER_BIT
    current_bit          = 0x00000001, // GL_CURRENT_BIT
    depth_buffer_bit     = 0x00000100, // GL_DEPTH_BUFFER_BIT
    enable_bit           = 0x00002000, // GL_ENABLE_BIT
    eval_bit             = 0x00010000, // GL_EVAL_BIT
    fog_bit              = 0x00000080, // GL_FOG_BIT
    hint_bit             = 0x00008000, // GL_HINT_BIT
    lighting_bit         = 0x00000040, // GL_LIGHTING_BIT
    line_bit             = 0x00000004, // GL_LINE_BIT
    list_bit             = 0x00020000, // GL_LIST_BIT
    multisample_bit      = 0x20000000, // GL_MULTISAMPLE_BIT
    pixel_mode_bit       = 0x00000020, // GL_PIXEL_MODE_BIT
    point_bit            = 0x00000002, // GL_POINT_BIT
    polygon_bit          = 0x00000008, // GL_POLYGON_BIT
    polygon_stipple_bit  = 0x00000010, // GL_POLYGON_STIPPLE_BIT
    scissor_bit          = 0x00080000, // GL_SCISSOR_BIT
    stencil_buffer_bit   = 0x00000400, // GL_STENCIL_BUFFER_BIT
    texture_bit          = 0x00040000, // GL_TEXTURE_BIT
    transform_bit        = 0x00001000, // GL_TRANSFORM_BIT
    viewport_bit         = 0x00000800, // GL_VIEWPORT_BIT
    multisample_bit_3dfx = 0x20000000, // GL_MULTISAMPLE_BIT_3DFX
    multisample_bit_arb  = 0x20000000, // GL_MULTISAMPLE_BIT_ARB
    multisample_bit_ext  = 0x20000000, // GL_MULTISAMPLE_BIT_EXT
}; // enum class attrib_mask
C_FLAGS(attrib_mask, u32);

} // namespace gl::group
