#pragma once

#ifdef GL_NV_multisample_coverage
namespace gl::nv::multisample_coverage {
namespace values {
constexpr u32 samples_arb   = 0x80A9;
constexpr u32 color_samples = 0x8E20;
} // namespace values
} // namespace gl::nv::multisample_coverage
#endif // GL_NV_multisample_coverage
namespace gl::nv::multisample_coverage {
constexpr auto name = "GL_NV_multisample_coverage";
}
