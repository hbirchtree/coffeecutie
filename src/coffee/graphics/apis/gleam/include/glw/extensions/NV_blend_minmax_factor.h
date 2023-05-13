#pragma once

#ifdef GL_NV_blend_minmax_factor
namespace gl::nv::blend_minmax_factor {
namespace values {
constexpr u32 factor_min_amd = 0x901C;
constexpr u32 factor_max_amd = 0x901D;
} // namespace values
} // namespace gl::nv::blend_minmax_factor
#endif // GL_NV_blend_minmax_factor
namespace gl::nv::blend_minmax_factor {
constexpr auto name = "GL_NV_blend_minmax_factor";
}
