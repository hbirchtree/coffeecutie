#pragma once

#ifdef GL_OES_shader_multisample_interpolation
namespace gl::oes::shader_multisample_interpolation {
namespace values {
constexpr u32 min_fragment_interpolation_offset  = 0x8E5B;
constexpr u32 max_fragment_interpolation_offset  = 0x8E5C;
constexpr u32 fragment_interpolation_offset_bits = 0x8E5D;
} // namespace values
} // namespace gl::oes::shader_multisample_interpolation
#endif // GL_OES_shader_multisample_interpolation
namespace gl::oes::shader_multisample_interpolation {
constexpr auto name = "GL_OES_shader_multisample_interpolation";
}
