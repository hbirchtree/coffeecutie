#pragma once

#include "common.h"

namespace gl::group {

// ElementPointerTypeATI
enum class element_pointer_type_ati : u32
{
#ifdef GL_UNSIGNED_BYTE
    unsigned_byte = GL_UNSIGNED_BYTE,
#endif
#ifdef GL_UNSIGNED_INT
    unsigned_int = GL_UNSIGNED_INT,
#endif
#ifdef GL_UNSIGNED_SHORT
    unsigned_short = GL_UNSIGNED_SHORT,
#endif
}; // enum class element_pointer_type_ati

} // namespace gl::group
