#pragma once

#ifdef GL_EXT_read_format_bgra
namespace gl::ext::read_format_bgra {
namespace values {
constexpr libc_types::u32 bgra                       = 0x80E1;
constexpr libc_types::u32 unsigned_short_4_4_4_4_rev = 0x8365;
constexpr libc_types::u32 unsigned_short_1_5_5_5_rev = 0x8366;
} // namespace values
} // namespace gl::ext::read_format_bgra
#endif // GL_EXT_read_format_bgra
namespace gl::ext::read_format_bgra {
constexpr auto name = "GL_EXT_read_format_bgra";
}
