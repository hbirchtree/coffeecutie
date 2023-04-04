#pragma once

#include "common.h"

namespace gl::group {

// FogCoordSrc
enum class fog_coord_src : u32
{
#ifdef GL_FOG_COORD
    fog_coord = GL_FOG_COORD,
#endif
#ifdef GL_FOG_COORDINATE
    fog_coordinate = GL_FOG_COORDINATE,
#endif
#ifdef GL_FOG_COORDINATE_EXT
    fog_coordinate_ext = GL_FOG_COORDINATE_EXT,
#endif
#ifdef GL_FRAGMENT_DEPTH
    fragment_depth = GL_FRAGMENT_DEPTH,
#endif
#ifdef GL_FRAGMENT_DEPTH_EXT
    fragment_depth_ext = GL_FRAGMENT_DEPTH_EXT,
#endif
}; // enum class fog_coord_src

} // namespace gl::group
