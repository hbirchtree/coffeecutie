#pragma once

#ifdef GL_NV_depth_clamp
namespace gl::nv::depth_clamp {
namespace values {
constexpr u32 depth_clamp = 0x864F;
} // namespace values
} // namespace gl::nv::depth_clamp
#endif // GL_NV_depth_clamp
namespace gl::nv::depth_clamp {
constexpr auto name = "GL_NV_depth_clamp";
}
