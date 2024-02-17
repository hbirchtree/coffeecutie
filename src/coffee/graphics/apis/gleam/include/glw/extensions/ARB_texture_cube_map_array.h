#pragma once

#ifdef GL_ARB_texture_cube_map_array
#include "../enums/TextureTarget.h"

namespace gl::arb::texture_cube_map_array {
using gl::group::texture_target;

namespace values {
constexpr u32 texture_binding_cube_map_array      = 0x900A;
constexpr u32 sampler_cube_map_array              = 0x900C;
constexpr u32 sampler_cube_map_array_shadow       = 0x900D;
constexpr u32 int_sampler_cube_map_array          = 0x900E;
constexpr u32 unsigned_int_sampler_cube_map_array = 0x900F;
} // namespace values
} // namespace gl::arb::texture_cube_map_array
#endif // GL_ARB_texture_cube_map_array
namespace gl::arb::texture_cube_map_array {
constexpr auto name = "GL_ARB_texture_cube_map_array";
}
