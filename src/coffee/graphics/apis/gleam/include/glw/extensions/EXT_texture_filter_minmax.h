#pragma once

#ifdef GL_EXT_texture_filter_minmax
namespace gl::ext::texture_filter_minmax {
namespace values {
constexpr u32 texture_reduction_mode = 0x9366;
constexpr u32 weighted_average       = 0x9367;
} // namespace values
} // namespace gl::ext::texture_filter_minmax
#endif // GL_EXT_texture_filter_minmax
namespace gl::ext::texture_filter_minmax {
constexpr auto name = "GL_EXT_texture_filter_minmax";
}
