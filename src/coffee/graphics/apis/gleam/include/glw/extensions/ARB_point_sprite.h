#pragma once

#ifdef GL_ARB_point_sprite
namespace gl::arb::point_sprite {
namespace values {
constexpr u32 point_sprite  = 0x8861;
constexpr u32 coord_replace = 0x8862;
} // namespace values
} // namespace gl::arb::point_sprite
#endif // GL_ARB_point_sprite
namespace gl::arb::point_sprite {
constexpr auto name = "GL_ARB_point_sprite";
}
