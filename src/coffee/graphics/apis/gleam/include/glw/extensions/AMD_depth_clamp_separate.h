#pragma once

#ifdef GL_AMD_depth_clamp_separate
namespace gl::amd::depth_clamp_separate {
namespace values {
constexpr libc_types::u32 depth_clamp_near = 0x901E;
constexpr libc_types::u32 depth_clamp_far  = 0x901F;
} // namespace values
} // namespace gl::amd::depth_clamp_separate
#endif // GL_AMD_depth_clamp_separate
namespace gl::amd::depth_clamp_separate {
constexpr auto name = "GL_AMD_depth_clamp_separate";
}
