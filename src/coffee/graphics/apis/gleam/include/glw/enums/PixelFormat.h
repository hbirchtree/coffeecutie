#pragma once

#include "common.h"

namespace gl::group {

// PixelFormat
enum class pixel_format : ::libc_types::u32
{
#ifdef GL_ABGR_EXT
    abgr_ext = GL_ABGR_EXT,
#endif
#ifdef GL_ALPHA
    alpha = GL_ALPHA,
#endif
#ifdef GL_BGR
    bgr = GL_BGR,
#endif
#ifdef GL_BGRA
    bgra = GL_BGRA,
#endif
#ifdef GL_BGRA_EXT
    bgra_ext = GL_BGRA_EXT,
#endif
#ifdef GL_BGRA_IMG
    bgra_img = GL_BGRA_IMG,
#endif
#ifdef GL_BGRA_INTEGER
    bgra_integer = GL_BGRA_INTEGER,
#endif
#ifdef GL_BGR_EXT
    bgr_ext = GL_BGR_EXT,
#endif
#ifdef GL_BGR_INTEGER
    bgr_integer = GL_BGR_INTEGER,
#endif
#ifdef GL_BLUE
    blue = GL_BLUE,
#endif
#ifdef GL_BLUE_INTEGER
    blue_integer = GL_BLUE_INTEGER,
#endif
#ifdef GL_CMYKA_EXT
    cmyka_ext = GL_CMYKA_EXT,
#endif
#ifdef GL_CMYK_EXT
    cmyk_ext = GL_CMYK_EXT,
#endif
#ifdef GL_COLOR_INDEX
    color_index = GL_COLOR_INDEX,
#endif
#ifdef GL_DEPTH_COMPONENT
    depth_component = GL_DEPTH_COMPONENT,
#endif
#ifdef GL_DEPTH_STENCIL
    depth_stencil = GL_DEPTH_STENCIL,
#endif
#ifdef GL_GREEN
    green = GL_GREEN,
#endif
#ifdef GL_GREEN_INTEGER
    green_integer = GL_GREEN_INTEGER,
#endif
#ifdef GL_LUMINANCE
    luminance = GL_LUMINANCE,
#endif
#ifdef GL_LUMINANCE_ALPHA
    luminance_alpha = GL_LUMINANCE_ALPHA,
#endif
#ifdef GL_RED
    red = GL_RED,
#endif
#ifdef GL_RED_EXT
    red_ext = GL_RED_EXT,
#endif
#ifdef GL_RED_INTEGER
    red_integer = GL_RED_INTEGER,
#endif
#ifdef GL_RG
    rg = GL_RG,
#endif
#ifdef GL_RGB
    rgb = GL_RGB,
#endif
#ifdef GL_RGBA
    rgba = GL_RGBA,
#endif
#ifdef GL_RGBA_INTEGER
    rgba_integer = GL_RGBA_INTEGER,
#endif
#ifdef GL_RGB_INTEGER
    rgb_integer = GL_RGB_INTEGER,
#endif
#ifdef GL_RG_INTEGER
    rg_integer = GL_RG_INTEGER,
#endif
#ifdef GL_STENCIL_INDEX
    stencil_index = GL_STENCIL_INDEX,
#endif
#ifdef GL_UNSIGNED_INT
    unsigned_int = GL_UNSIGNED_INT,
#endif
#ifdef GL_UNSIGNED_SHORT
    unsigned_short = GL_UNSIGNED_SHORT,
#endif
#ifdef GL_YCRCB_422_SGIX
    ycrcb_422_sgix = GL_YCRCB_422_SGIX,
#endif
#ifdef GL_YCRCB_444_SGIX
    ycrcb_444_sgix = GL_YCRCB_444_SGIX,
#endif
}; // enum class pixel_format

} // namespace gl::group
