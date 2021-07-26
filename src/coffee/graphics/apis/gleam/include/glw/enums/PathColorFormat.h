#pragma once

#include "common.h"

namespace gl::group {

// PathColorFormat
enum class path_color_format : ::libc_types::u32
{
#ifdef GL_ALPHA
    alpha = GL_ALPHA,
#endif
#ifdef GL_INTENSITY
    intensity = GL_INTENSITY,
#endif
#ifdef GL_LUMINANCE
    luminance = GL_LUMINANCE,
#endif
#ifdef GL_LUMINANCE_ALPHA
    luminance_alpha = GL_LUMINANCE_ALPHA,
#endif
#ifdef GL_NONE
    none = GL_NONE,
#endif
#ifdef GL_RGB
    rgb = GL_RGB,
#endif
#ifdef GL_RGBA
    rgba = GL_RGBA,
#endif
}; // enum class path_color_format

} // namespace gl::group
