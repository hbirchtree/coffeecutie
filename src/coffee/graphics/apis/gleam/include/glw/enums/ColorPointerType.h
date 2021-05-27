#pragma once

#include "common.h"

namespace gl::groups {

// ColorPointerType
enum class color_pointer_type : ::libc_types::u32 {
#ifdef GL_BYTE
    byte_ = GL_BYTE,
#endif
#ifdef GL_UNSIGNED_BYTE
    unsigned_byte = GL_UNSIGNED_BYTE,
#endif
#ifdef GL_UNSIGNED_INT
    unsigned_int = GL_UNSIGNED_INT,
#endif
#ifdef GL_UNSIGNED_SHORT
    unsigned_short = GL_UNSIGNED_SHORT,
#endif
}; // enum class color_pointer_type

} // namespace gl::groups
