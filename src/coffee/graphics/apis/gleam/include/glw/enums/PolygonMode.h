#pragma once

#include "common.h"

namespace gl::group {

// PolygonMode
enum class polygon_mode : u32
{
    fill  = 0x1B02, // GL_FILL
    line  = 0x1B01, // GL_LINE
    point = 0x1B00, // GL_POINT
}; // enum class polygon_mode

} // namespace gl::group
