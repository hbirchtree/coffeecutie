#pragma once

#ifdef GL_EXT_texture_cube_map
#include "../enums/TextureTarget.h"
namespace gl::ext::texture_cube_map {
using gl::group::texture_target;
namespace values {
constexpr libc_types::u32 normal_map                  = 0x8511;
constexpr libc_types::u32 reflection_map              = 0x8512;
constexpr libc_types::u32 texture_cube_map            = 0x8513;
constexpr libc_types::u32 texture_binding_cube_map    = 0x8514;
constexpr libc_types::u32 texture_cube_map_positive_x = 0x8515;
constexpr libc_types::u32 texture_cube_map_negative_x = 0x8516;
constexpr libc_types::u32 texture_cube_map_positive_y = 0x8517;
constexpr libc_types::u32 texture_cube_map_negative_y = 0x8518;
constexpr libc_types::u32 texture_cube_map_positive_z = 0x8519;
constexpr libc_types::u32 texture_cube_map_negative_z = 0x851A;
constexpr libc_types::u32 max_cube_map_texture_size   = 0x851C;
} // namespace values
} // namespace gl::ext::texture_cube_map
#endif // GL_EXT_texture_cube_map
namespace gl::ext::texture_cube_map {
constexpr auto name = "GL_EXT_texture_cube_map";
}
