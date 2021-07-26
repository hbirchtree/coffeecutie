#pragma once

#ifdef GL_AMD_compressed_3DC_texture
namespace gl::amd::compressed_3dc_texture {
namespace values {
constexpr libc_types::u32 n3dc_x  = 0x87F9;
constexpr libc_types::u32 n3dc_xy = 0x87FA;
} // namespace values
} // namespace gl::amd::compressed_3dc_texture
#endif // GL_AMD_compressed_3DC_texture
namespace gl::amd::compressed_3dc_texture {
constexpr auto name = "GL_AMD_compressed_3DC_texture";
}
