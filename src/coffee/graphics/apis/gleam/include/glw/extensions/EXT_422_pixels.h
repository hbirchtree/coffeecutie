#pragma once

#ifdef GL_EXT_422_pixels
namespace gl::ext::422_pixels
{
    namespace values {
    constexpr libc_types::u32 n422             = 0x80CC;
    constexpr libc_types::u32 n422_rev         = 0x80CD;
    constexpr libc_types::u32 n422_average     = 0x80CE;
    constexpr libc_types::u32 n422_rev_average = 0x80CF;
    } // namespace values
} // namespace gl::ext::422_pixels
#endif // GL_EXT_422_pixels
namespace gl::ext::422_pixels
{
    constexpr auto name = "GL_EXT_422_pixels";
}