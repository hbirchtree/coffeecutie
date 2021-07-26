#pragma once

#include "common.h"

namespace gl::group {

// PathElementType
enum class path_element_type : ::libc_types::u32
{
#ifdef GL_UTF16_NV
    utf16_nv = GL_UTF16_NV,
#endif
#ifdef GL_UTF8_NV
    utf8_nv = GL_UTF8_NV,
#endif
}; // enum class path_element_type

} // namespace gl::group
