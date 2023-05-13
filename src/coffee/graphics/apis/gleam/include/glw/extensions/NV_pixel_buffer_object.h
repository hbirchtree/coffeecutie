#pragma once

#ifdef GL_NV_pixel_buffer_object
namespace gl::nv::pixel_buffer_object {
namespace values {
constexpr u32 pixel_pack_buffer           = 0x88EB;
constexpr u32 pixel_unpack_buffer         = 0x88EC;
constexpr u32 pixel_pack_buffer_binding   = 0x88ED;
constexpr u32 pixel_unpack_buffer_binding = 0x88EF;
} // namespace values
} // namespace gl::nv::pixel_buffer_object
#endif // GL_NV_pixel_buffer_object
namespace gl::nv::pixel_buffer_object {
constexpr auto name = "GL_NV_pixel_buffer_object";
}
