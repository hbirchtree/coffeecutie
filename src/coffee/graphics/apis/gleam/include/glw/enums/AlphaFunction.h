#pragma once

#include "common.h"

namespace gl::group {

// AlphaFunction
enum class alpha_function : u32
{
    always   = 0x0207, // GL_ALWAYS
    equal    = 0x0202, // GL_EQUAL
    gequal   = 0x0206, // GL_GEQUAL
    greater  = 0x0204, // GL_GREATER
    lequal   = 0x0203, // GL_LEQUAL
    less     = 0x0201, // GL_LESS
    never    = 0x0200, // GL_NEVER
    notequal = 0x0205, // GL_NOTEQUAL
}; // enum class alpha_function

} // namespace gl::group
