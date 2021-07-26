#pragma once

#ifdef GL_IMG_read_format
namespace gl::img::read_format {
namespace values {
constexpr libc_types::u32 bgra                       = 0x80E1;
constexpr libc_types::u32 unsigned_short_4_4_4_4_rev = 0x8365;
} // namespace values
} // namespace gl::img::read_format
#endif // GL_IMG_read_format
namespace gl::img::read_format {
constexpr auto name = "GL_IMG_read_format";
}
