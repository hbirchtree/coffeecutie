#pragma once

#ifdef GL_ARB_cull_distance
namespace gl::arb::cull_distance {
namespace values {
constexpr u32 max_cull_distances                   = 0x82F9;
constexpr u32 max_combined_clip_and_cull_distances = 0x82FA;
} // namespace values
} // namespace gl::arb::cull_distance
#endif // GL_ARB_cull_distance
namespace gl::arb::cull_distance {
constexpr auto name = "GL_ARB_cull_distance";
}
