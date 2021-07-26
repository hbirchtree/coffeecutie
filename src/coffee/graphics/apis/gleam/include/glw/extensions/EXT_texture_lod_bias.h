#pragma once

#ifdef GL_EXT_texture_lod_bias
namespace gl::ext::texture_lod_bias {
namespace values {
constexpr libc_types::u32 max_texture_lod_bias   = 0x84FD;
constexpr libc_types::u32 texture_filter_control = 0x8500;
constexpr libc_types::u32 texture_lod_bias       = 0x8501;
} // namespace values
} // namespace gl::ext::texture_lod_bias
#endif // GL_EXT_texture_lod_bias
namespace gl::ext::texture_lod_bias {
constexpr auto name = "GL_EXT_texture_lod_bias";
}
