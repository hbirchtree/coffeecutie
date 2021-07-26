#pragma once

#ifdef GL_NV_copy_depth_to_color
namespace gl::nv::copy_depth_to_color {
namespace values {
constexpr libc_types::u32 depth_stencil_to_rgba = 0x886E;
constexpr libc_types::u32 depth_stencil_to_bgra = 0x886F;
} // namespace values
} // namespace gl::nv::copy_depth_to_color
#endif // GL_NV_copy_depth_to_color
namespace gl::nv::copy_depth_to_color {
constexpr auto name = "GL_NV_copy_depth_to_color";
}
