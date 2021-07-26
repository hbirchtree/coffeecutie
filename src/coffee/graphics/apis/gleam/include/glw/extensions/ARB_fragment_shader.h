#pragma once

#ifdef GL_ARB_fragment_shader
#include "../enums/HintTarget.h"
#include "../enums/ShaderType.h"
namespace gl::arb::fragment_shader {
using gl::group::hint_target;
using gl::group::shader_type;
namespace values {
constexpr libc_types::u32 max_fragment_uniform_components = 0x8B49;
} // namespace values
} // namespace gl::arb::fragment_shader
#endif // GL_ARB_fragment_shader
namespace gl::arb::fragment_shader {
constexpr auto name = "GL_ARB_fragment_shader";
}
