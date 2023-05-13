#pragma once

#ifdef GL_NV_deep_texture3D
namespace gl::nv::deep_texture3d {
namespace values {
constexpr u32 max_deep_3d_texture_width_height = 0x90D0;
constexpr u32 max_deep_3d_texture_depth        = 0x90D1;
} // namespace values
} // namespace gl::nv::deep_texture3d
#endif // GL_NV_deep_texture3D
namespace gl::nv::deep_texture3d {
constexpr auto name = "GL_NV_deep_texture3D";
}
