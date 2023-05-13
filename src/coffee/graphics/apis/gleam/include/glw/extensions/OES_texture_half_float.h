#pragma once

#ifdef GL_OES_texture_half_float
namespace gl::oes::texture_half_float {
namespace values {
constexpr u32 half_float = 0x8D61;
} // namespace values
} // namespace gl::oes::texture_half_float
#endif // GL_OES_texture_half_float
namespace gl::oes::texture_half_float {
constexpr auto name = "GL_OES_texture_half_float";
}
