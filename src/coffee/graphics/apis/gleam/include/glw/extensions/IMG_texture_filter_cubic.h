#pragma once

#ifdef GL_IMG_texture_filter_cubic
namespace gl::img::texture_filter_cubic {
namespace values {
constexpr libc_types::u32 cubic                = 0x9139;
constexpr libc_types::u32 cubic_mipmap_nearest = 0x913A;
constexpr libc_types::u32 cubic_mipmap_linear  = 0x913B;
} // namespace values
} // namespace gl::img::texture_filter_cubic
#endif // GL_IMG_texture_filter_cubic
namespace gl::img::texture_filter_cubic {
constexpr auto name = "GL_IMG_texture_filter_cubic";
}
