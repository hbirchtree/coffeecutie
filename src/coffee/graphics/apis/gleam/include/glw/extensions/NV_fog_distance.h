#pragma once

#ifdef GL_NV_fog_distance
#include "../enums/TextureGenParameter.h"

namespace gl::nv::fog_distance {
using gl::group::texture_gen_parameter;

namespace values {
constexpr u32 fog_distance_mode  = 0x855A;
constexpr u32 eye_radial         = 0x855B;
constexpr u32 eye_plane_absolute = 0x855C;
} // namespace values
} // namespace gl::nv::fog_distance
#endif // GL_NV_fog_distance
namespace gl::nv::fog_distance {
constexpr auto name = "GL_NV_fog_distance";
}
