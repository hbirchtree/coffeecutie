#pragma once

#ifdef GL_NV_depth_nonlinear
namespace gl::nv::depth_nonlinear {
namespace values {
constexpr libc_types::u32 depth_component16_nonlinear = 0x8E2C;
} // namespace values
} // namespace gl::nv::depth_nonlinear
#endif // GL_NV_depth_nonlinear
namespace gl::nv::depth_nonlinear {
constexpr auto name = "GL_NV_depth_nonlinear";
}
