#pragma once

#ifdef GL_ARM_mali_shader_binary
#include "../enums/ShaderBinaryFormat.h"
namespace gl::arm::mali_shader_binary {
using gl::group::shader_binary_format;
namespace values {
} // namespace values
} // namespace gl::arm::mali_shader_binary
#endif // GL_ARM_mali_shader_binary
namespace gl::arm::mali_shader_binary {
constexpr auto name = "GL_ARM_mali_shader_binary";
}
