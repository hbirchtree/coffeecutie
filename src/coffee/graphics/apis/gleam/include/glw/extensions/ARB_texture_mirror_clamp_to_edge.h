#pragma once

#ifdef GL_ARB_texture_mirror_clamp_to_edge
namespace gl::arb::texture_mirror_clamp_to_edge {
namespace values {
constexpr u32 mirror_clamp_to_edge = 0x8743;
} // namespace values
} // namespace gl::arb::texture_mirror_clamp_to_edge
#endif // GL_ARB_texture_mirror_clamp_to_edge
namespace gl::arb::texture_mirror_clamp_to_edge {
constexpr auto name = "GL_ARB_texture_mirror_clamp_to_edge";
}
