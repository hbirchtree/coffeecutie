#pragma once

#ifdef GL_AMD_blend_minmax_factor
namespace gl::amd::blend_minmax_factor {
namespace values {
constexpr u32 factor_min = 0x901C;
constexpr u32 factor_max = 0x901D;
} // namespace values
} // namespace gl::amd::blend_minmax_factor
#endif // GL_AMD_blend_minmax_factor
namespace gl::amd::blend_minmax_factor {
constexpr auto name = "GL_AMD_blend_minmax_factor";
}
