#pragma once

#ifdef GL_EXT_pixel_buffer_object
namespace gl::ext::pixel_buffer_object {
namespace values {
constexpr libc_types::u32 pixel_pack_buffer           = 0x88EB;
constexpr libc_types::u32 pixel_unpack_buffer         = 0x88EC;
constexpr libc_types::u32 pixel_pack_buffer_binding   = 0x88ED;
constexpr libc_types::u32 pixel_unpack_buffer_binding = 0x88EF;
} // namespace values
} // namespace gl::ext::pixel_buffer_object
#endif // GL_EXT_pixel_buffer_object
namespace gl::ext::pixel_buffer_object {
constexpr auto name = "GL_EXT_pixel_buffer_object";
}
