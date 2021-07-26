#pragma once

#ifdef GL_OES_texture_cube_map_array
#include "../enums/TextureTarget.h"
namespace gl::oes::texture_cube_map_array {
using gl::group::texture_target;
namespace values {
constexpr libc_types::u32 texture_binding_cube_map_array      = 0x900A;
constexpr libc_types::u32 sampler_cube_map_array              = 0x900C;
constexpr libc_types::u32 sampler_cube_map_array_shadow       = 0x900D;
constexpr libc_types::u32 int_sampler_cube_map_array          = 0x900E;
constexpr libc_types::u32 unsigned_int_sampler_cube_map_array = 0x900F;
constexpr libc_types::u32 image_cube_map_array                = 0x9054;
constexpr libc_types::u32 int_image_cube_map_array            = 0x905F;
constexpr libc_types::u32 unsigned_int_image_cube_map_array   = 0x906A;
} // namespace values
} // namespace gl::oes::texture_cube_map_array
#endif // GL_OES_texture_cube_map_array
namespace gl::oes::texture_cube_map_array {
constexpr auto name = "GL_OES_texture_cube_map_array";
}
