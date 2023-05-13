#pragma once

#ifdef GL_EXT_shader_pixel_local_storage
namespace gl::ext::shader_pixel_local_storage {
namespace values {
constexpr u32 max_shader_pixel_local_storage_fast_size = 0x8F63;
constexpr u32 shader_pixel_local_storage               = 0x8F64;
constexpr u32 max_shader_pixel_local_storage_size      = 0x8F67;
} // namespace values
} // namespace gl::ext::shader_pixel_local_storage
#endif // GL_EXT_shader_pixel_local_storage
namespace gl::ext::shader_pixel_local_storage {
constexpr auto name = "GL_EXT_shader_pixel_local_storage";
}
