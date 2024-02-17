#pragma once

#ifdef GL_EXT_blend_subtract
#include "../enums/BlendEquationModeEXT.h"

namespace gl::ext::blend_subtract {
using gl::group::blend_equation_mode_ext;

namespace values {
} // namespace values
} // namespace gl::ext::blend_subtract
#endif // GL_EXT_blend_subtract
namespace gl::ext::blend_subtract {
constexpr auto name = "GL_EXT_blend_subtract";
}
