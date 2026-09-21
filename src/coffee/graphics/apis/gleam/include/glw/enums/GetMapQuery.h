#pragma once

#include "common.h"

namespace gl::group {

// GetMapQuery
enum class get_map_query : u32
{
    coeff  = 0x0A00, // GL_COEFF
    domain = 0x0A02, // GL_DOMAIN
    order  = 0x0A01, // GL_ORDER
}; // enum class get_map_query

} // namespace gl::group
