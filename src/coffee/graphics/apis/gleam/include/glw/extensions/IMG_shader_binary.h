#pragma once

#ifdef GL_IMG_shader_binary
#include "../enums/ShaderBinaryFormat.h"
namespace gl::img::shader_binary {
using gl::group::shader_binary_format;
namespace values {
} // namespace values
} // namespace gl::img::shader_binary
#endif // GL_IMG_shader_binary
namespace gl::img::shader_binary {
constexpr auto name = "GL_IMG_shader_binary";
}
