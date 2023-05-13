#pragma once

#ifdef GL_OES_read_format
namespace gl::oes::read_format {
namespace values {
constexpr u32 implementation_color_read_type   = 0x8B9A;
constexpr u32 implementation_color_read_format = 0x8B9B;
} // namespace values
} // namespace gl::oes::read_format
#endif // GL_OES_read_format
namespace gl::oes::read_format {
constexpr auto name = "GL_OES_read_format";
}
