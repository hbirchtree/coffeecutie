#pragma once

#include "common.h"

namespace gl::group {

// BlendEquationModeEXT
enum class blend_equation_mode_ext : u32
{
    func_add                  = 0x8006, // GL_FUNC_ADD
    func_reverse_subtract     = 0x800B, // GL_FUNC_REVERSE_SUBTRACT
    func_subtract             = 0x800A, // GL_FUNC_SUBTRACT
    max                       = 0x8008, // GL_MAX
    min                       = 0x8007, // GL_MIN
    alpha_max_sgix            = 0x8321, // GL_ALPHA_MAX_SGIX
    alpha_min_sgix            = 0x8320, // GL_ALPHA_MIN_SGIX
    func_add_ext              = 0x8006, // GL_FUNC_ADD_EXT
    func_reverse_subtract_ext = 0x800B, // GL_FUNC_REVERSE_SUBTRACT_EXT
    func_subtract_ext         = 0x800A, // GL_FUNC_SUBTRACT_EXT
    max_ext                   = 0x8008, // GL_MAX_EXT
    min_ext                   = 0x8007, // GL_MIN_EXT
}; // enum class blend_equation_mode_ext

} // namespace gl::group
