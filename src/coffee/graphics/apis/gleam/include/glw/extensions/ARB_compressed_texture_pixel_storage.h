#pragma once

#ifdef GL_ARB_compressed_texture_pixel_storage
namespace gl::arb::compressed_texture_pixel_storage {
namespace values {
constexpr libc_types::u32 unpack_compressed_block_width  = 0x9127;
constexpr libc_types::u32 unpack_compressed_block_height = 0x9128;
constexpr libc_types::u32 unpack_compressed_block_depth  = 0x9129;
constexpr libc_types::u32 unpack_compressed_block_size   = 0x912A;
constexpr libc_types::u32 pack_compressed_block_width    = 0x912B;
constexpr libc_types::u32 pack_compressed_block_height   = 0x912C;
constexpr libc_types::u32 pack_compressed_block_depth    = 0x912D;
constexpr libc_types::u32 pack_compressed_block_size     = 0x912E;
} // namespace values
} // namespace gl::arb::compressed_texture_pixel_storage
#endif // GL_ARB_compressed_texture_pixel_storage
namespace gl::arb::compressed_texture_pixel_storage {
constexpr auto name = "GL_ARB_compressed_texture_pixel_storage";
}
