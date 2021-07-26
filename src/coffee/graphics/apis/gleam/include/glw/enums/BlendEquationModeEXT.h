#pragma once

#include "common.h"

namespace gl::group {

// BlendEquationModeEXT
enum class blend_equation_mode_ext : ::libc_types::u32
{
#ifdef GL_ALPHA_MAX_SGIX
    alpha_max_sgix = GL_ALPHA_MAX_SGIX,
#endif
#ifdef GL_ALPHA_MIN_SGIX
    alpha_min_sgix = GL_ALPHA_MIN_SGIX,
#endif
#ifdef GL_FUNC_ADD
    func_add = GL_FUNC_ADD,
#endif
#ifdef GL_FUNC_ADD_EXT
    func_add_ext = GL_FUNC_ADD_EXT,
#endif
#ifdef GL_FUNC_REVERSE_SUBTRACT
    func_reverse_subtract = GL_FUNC_REVERSE_SUBTRACT,
#endif
#ifdef GL_FUNC_REVERSE_SUBTRACT_EXT
    func_reverse_subtract_ext = GL_FUNC_REVERSE_SUBTRACT_EXT,
#endif
#ifdef GL_FUNC_SUBTRACT
    func_subtract = GL_FUNC_SUBTRACT,
#endif
#ifdef GL_FUNC_SUBTRACT_EXT
    func_subtract_ext = GL_FUNC_SUBTRACT_EXT,
#endif
#ifdef GL_MAX
    max = GL_MAX,
#endif
#ifdef GL_MAX_EXT
    max_ext = GL_MAX_EXT,
#endif
#ifdef GL_MIN
    min = GL_MIN,
#endif
#ifdef GL_MIN_EXT
    min_ext = GL_MIN_EXT,
#endif
}; // enum class blend_equation_mode_ext

} // namespace gl::group
