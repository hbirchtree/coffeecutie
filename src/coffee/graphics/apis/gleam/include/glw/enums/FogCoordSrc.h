#pragma once

#include "common.h"

namespace gl::group {

// FogCoordSrc
enum class fog_coord_src : u32
{
    fog_coord          = 0x8451, // GL_FOG_COORD
    fog_coordinate     = 0x8451, // GL_FOG_COORDINATE
    fragment_depth     = 0x8452, // GL_FRAGMENT_DEPTH
    fog_coordinate_ext = 0x8451, // GL_FOG_COORDINATE_EXT
    fragment_depth_ext = 0x8452, // GL_FRAGMENT_DEPTH_EXT
}; // enum class fog_coord_src

} // namespace gl::group
