#pragma once

#include "common.h"

namespace gl::group {

// StringName
enum class string_name : u32
{
#ifdef GL_EXTENSIONS
    extensions = GL_EXTENSIONS,
#endif
#ifdef GL_RENDERER
    renderer = GL_RENDERER,
#endif
#ifdef GL_SHADING_LANGUAGE_VERSION
    shading_language_version = GL_SHADING_LANGUAGE_VERSION,
#endif
#ifdef GL_VENDOR
    vendor = GL_VENDOR,
#endif
#ifdef GL_VERSION
    version = GL_VERSION,
#endif
}; // enum class string_name

} // namespace gl::group
