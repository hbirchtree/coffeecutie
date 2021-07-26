#pragma once

#ifdef GL_EXT_texture_filter_anisotropic
namespace gl::ext::texture_filter_anisotropic {
namespace values {
constexpr libc_types::u32 texture_max_anisotropy     = 0x84FE;
constexpr libc_types::u32 max_texture_max_anisotropy = 0x84FF;
} // namespace values
} // namespace gl::ext::texture_filter_anisotropic
#endif // GL_EXT_texture_filter_anisotropic
namespace gl::ext::texture_filter_anisotropic {
constexpr auto name = "GL_EXT_texture_filter_anisotropic";
}
