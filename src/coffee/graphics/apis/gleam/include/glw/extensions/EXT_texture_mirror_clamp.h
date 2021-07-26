#pragma once

#ifdef GL_EXT_texture_mirror_clamp
namespace gl::ext::texture_mirror_clamp {
namespace values {
constexpr libc_types::u32 mirror_clamp           = 0x8742;
constexpr libc_types::u32 mirror_clamp_to_edge   = 0x8743;
constexpr libc_types::u32 mirror_clamp_to_border = 0x8912;
} // namespace values
} // namespace gl::ext::texture_mirror_clamp
#endif // GL_EXT_texture_mirror_clamp
namespace gl::ext::texture_mirror_clamp {
constexpr auto name = "GL_EXT_texture_mirror_clamp";
}
