#pragma once

#ifdef GL_INTEL_conservative_rasterization
namespace gl::intel::conservative_rasterization {
namespace values {
constexpr libc_types::u32 conservative_rasterization = 0x83FE;
} // namespace values
} // namespace gl::intel::conservative_rasterization
#endif // GL_INTEL_conservative_rasterization
namespace gl::intel::conservative_rasterization {
constexpr auto name = "GL_INTEL_conservative_rasterization";
}
