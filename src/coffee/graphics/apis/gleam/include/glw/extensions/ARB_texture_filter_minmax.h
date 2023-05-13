#pragma once

#ifdef GL_ARB_texture_filter_minmax
namespace gl::arb::texture_filter_minmax {
namespace values {
constexpr u32 texture_reduction_mode = 0x9366;
constexpr u32 weighted_average       = 0x9367;
} // namespace values
} // namespace gl::arb::texture_filter_minmax
#endif // GL_ARB_texture_filter_minmax
namespace gl::arb::texture_filter_minmax {
constexpr auto name = "GL_ARB_texture_filter_minmax";
}
