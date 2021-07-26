#pragma once

#ifdef GL_NV_shadow_samplers_cube
namespace gl::nv::shadow_samplers_cube {
namespace values {
constexpr libc_types::u32 sampler_cube_shadow = 0x8DC5;
} // namespace values
} // namespace gl::nv::shadow_samplers_cube
#endif // GL_NV_shadow_samplers_cube
namespace gl::nv::shadow_samplers_cube {
constexpr auto name = "GL_NV_shadow_samplers_cube";
}
