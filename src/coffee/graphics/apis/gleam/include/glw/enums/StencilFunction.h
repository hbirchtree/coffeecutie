#pragma once

#include "common.h"

namespace gl::group {

// StencilFunction
enum class stencil_function : u32
{
#ifdef GL_ALWAYS
    always = GL_ALWAYS,
#endif
#ifdef GL_EQUAL
    equal = GL_EQUAL,
#endif
#ifdef GL_GEQUAL
    gequal = GL_GEQUAL,
#endif
#ifdef GL_GREATER
    greater = GL_GREATER,
#endif
#ifdef GL_LEQUAL
    lequal = GL_LEQUAL,
#endif
#ifdef GL_LESS
    less = GL_LESS,
#endif
#ifdef GL_NEVER
    never = GL_NEVER,
#endif
#ifdef GL_NOTEQUAL
    notequal = GL_NOTEQUAL,
#endif
}; // enum class stencil_function

} // namespace gl::group
