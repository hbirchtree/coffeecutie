#pragma once

#ifdef GL_AMD_compressed_ATC_texture
namespace gl::amd::compressed_atc_texture {
namespace values {
constexpr u32 atc_rgba_interpolated_alpha = 0x87EE;
constexpr u32 atc_rgb                     = 0x8C92;
constexpr u32 atc_rgba_explicit_alpha     = 0x8C93;
} // namespace values
} // namespace gl::amd::compressed_atc_texture
#endif // GL_AMD_compressed_ATC_texture
namespace gl::amd::compressed_atc_texture {
constexpr auto name = "GL_AMD_compressed_ATC_texture";
}
