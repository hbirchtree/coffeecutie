#pragma once

#ifdef GL_AMD_gpu_shader_half_float
namespace gl::amd::gpu_shader_half_float {
namespace values {
constexpr u32 float16_nv      = 0x8FF8;
constexpr u32 float16_vec2_nv = 0x8FF9;
constexpr u32 float16_vec3_nv = 0x8FFA;
constexpr u32 float16_vec4_nv = 0x8FFB;
constexpr u32 float16_mat2    = 0x91C5;
constexpr u32 float16_mat3    = 0x91C6;
constexpr u32 float16_mat4    = 0x91C7;
constexpr u32 float16_mat2x3  = 0x91C8;
constexpr u32 float16_mat2x4  = 0x91C9;
constexpr u32 float16_mat3x2  = 0x91CA;
constexpr u32 float16_mat3x4  = 0x91CB;
constexpr u32 float16_mat4x2  = 0x91CC;
constexpr u32 float16_mat4x3  = 0x91CD;
} // namespace values
} // namespace gl::amd::gpu_shader_half_float
#endif // GL_AMD_gpu_shader_half_float
namespace gl::amd::gpu_shader_half_float {
constexpr auto name = "GL_AMD_gpu_shader_half_float";
}
