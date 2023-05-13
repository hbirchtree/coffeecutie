#pragma once

#ifdef GL_EXT_texture_mirror_clamp_to_edge
namespace gl::ext::texture_mirror_clamp_to_edge {
namespace values {
constexpr u32 mirror_clamp_to_edge = 0x8743;
} // namespace values
} // namespace gl::ext::texture_mirror_clamp_to_edge
#endif // GL_EXT_texture_mirror_clamp_to_edge
namespace gl::ext::texture_mirror_clamp_to_edge {
constexpr auto name = "GL_EXT_texture_mirror_clamp_to_edge";
}
