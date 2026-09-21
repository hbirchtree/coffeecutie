#pragma once

#include "common.h"

namespace gl::group {

// PathElementType
enum class path_element_type : u32
{
    utf16_nv = 0x909B, // GL_UTF16_NV
    utf8_nv  = 0x909A, // GL_UTF8_NV
}; // enum class path_element_type

} // namespace gl::group
