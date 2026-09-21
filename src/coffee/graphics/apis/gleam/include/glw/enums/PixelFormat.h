#pragma once

#include "common.h"

namespace gl::group {

// PixelFormat
enum class pixel_format : u32
{
    alpha           = 0x1906, // GL_ALPHA
    bgr             = 0x80E0, // GL_BGR
    bgra            = 0x80E1, // GL_BGRA
    bgra_integer    = 0x8D9B, // GL_BGRA_INTEGER
    bgr_integer     = 0x8D9A, // GL_BGR_INTEGER
    blue            = 0x1905, // GL_BLUE
    blue_integer    = 0x8D96, // GL_BLUE_INTEGER
    color_index     = 0x1900, // GL_COLOR_INDEX
    depth_component = 0x1902, // GL_DEPTH_COMPONENT
    depth_stencil   = 0x84F9, // GL_DEPTH_STENCIL
    green           = 0x1904, // GL_GREEN
    green_integer   = 0x8D95, // GL_GREEN_INTEGER
    luminance       = 0x1909, // GL_LUMINANCE
    luminance_alpha = 0x190A, // GL_LUMINANCE_ALPHA
    red             = 0x1903, // GL_RED
    red_integer     = 0x8D94, // GL_RED_INTEGER
    rg              = 0x8227, // GL_RG
    rgb             = 0x1907, // GL_RGB
    rgba            = 0x1908, // GL_RGBA
    rgba_integer    = 0x8D99, // GL_RGBA_INTEGER
    rgb_integer     = 0x8D98, // GL_RGB_INTEGER
    rg_integer      = 0x8228, // GL_RG_INTEGER
    stencil_index   = 0x1901, // GL_STENCIL_INDEX
    unsigned_int    = 0x1405, // GL_UNSIGNED_INT
    unsigned_short  = 0x1403, // GL_UNSIGNED_SHORT
    abgr_ext        = 0x8000, // GL_ABGR_EXT
    bgra_ext        = 0x80E1, // GL_BGRA_EXT
    bgra_img        = 0x80E1, // GL_BGRA_IMG
    bgr_ext         = 0x80E0, // GL_BGR_EXT
    cmyka_ext       = 0x800D, // GL_CMYKA_EXT
    cmyk_ext        = 0x800C, // GL_CMYK_EXT
    red_ext         = 0x1903, // GL_RED_EXT
    ycrcb_422_sgix  = 0x81BB, // GL_YCRCB_422_SGIX
    ycrcb_444_sgix  = 0x81BC, // GL_YCRCB_444_SGIX
}; // enum class pixel_format

} // namespace gl::group
