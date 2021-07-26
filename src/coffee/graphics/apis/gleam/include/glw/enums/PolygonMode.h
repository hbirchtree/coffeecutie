#pragma once

#include "common.h"

namespace gl::group {

// PolygonMode
enum class polygon_mode : ::libc_types::u32
{
#ifdef GL_FILL
    fill = GL_FILL,
#endif
#ifdef GL_LINE
    line = GL_LINE,
#endif
#ifdef GL_POINT
    point = GL_POINT,
#endif
}; // enum class polygon_mode

} // namespace gl::group
