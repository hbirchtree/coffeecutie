#pragma once

#ifdef GL_EXT_texture_format_BGRA8888
namespace gl::ext::texture_format_bgra8888 {
namespace values {
constexpr libc_types::u32 bgra = 0x80E1;
} // namespace values
} // namespace gl::ext::texture_format_bgra8888
#endif // GL_EXT_texture_format_BGRA8888
namespace gl::ext::texture_format_bgra8888 {
constexpr auto name = "GL_EXT_texture_format_BGRA8888";
}
