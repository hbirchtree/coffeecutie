#pragma once

#include "common.h"

namespace gl::group {

// StringName
enum class string_name : u32
{
    extensions               = 0x1F03, // GL_EXTENSIONS
    renderer                 = 0x1F01, // GL_RENDERER
    shading_language_version = 0x8B8C, // GL_SHADING_LANGUAGE_VERSION
    vendor                   = 0x1F00, // GL_VENDOR
    version                  = 0x1F02, // GL_VERSION
}; // enum class string_name

} // namespace gl::group
