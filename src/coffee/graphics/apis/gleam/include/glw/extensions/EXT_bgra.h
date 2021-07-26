#pragma once

#ifdef GL_EXT_bgra
namespace gl::ext::bgra {
namespace values {
constexpr libc_types::u32 bgr  = 0x80E0;
constexpr libc_types::u32 bgra = 0x80E1;
} // namespace values
} // namespace gl::ext::bgra
#endif // GL_EXT_bgra
namespace gl::ext::bgra {
constexpr auto name = "GL_EXT_bgra";
}
