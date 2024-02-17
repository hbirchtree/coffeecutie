#pragma once

#ifdef GL_EXT_texture_cube_map_array
#include "../enums/TextureTarget.h"

namespace gl::ext::texture_cube_map_array {
using gl::group::texture_target;

namespace values {
constexpr u32 texture_binding_cube_map_array      = 0x900A;
constexpr u32 sampler_cube_map_array              = 0x900C;
constexpr u32 sampler_cube_map_array_shadow       = 0x900D;
constexpr u32 int_sampler_cube_map_array          = 0x900E;
constexpr u32 unsigned_int_sampler_cube_map_array = 0x900F;
constexpr u32 image_cube_map_array                = 0x9054;
constexpr u32 int_image_cube_map_array            = 0x905F;
constexpr u32 unsigned_int_image_cube_map_array   = 0x906A;
} // namespace values
} // namespace gl::ext::texture_cube_map_array
#endif // GL_EXT_texture_cube_map_array
namespace gl::ext::texture_cube_map_array {
constexpr auto name = "GL_EXT_texture_cube_map_array";
}
